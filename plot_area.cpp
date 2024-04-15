#include "plot_area.hpp"
#include <charconv>

namespace pinscope {

PlotArea::PlotArea(const pinscope::Window *data, const bool *enabled,
                   size_t pin_cnt, const int *time_span)
    : data_(data), enabled_(enabled), pin_cnt_(pin_cnt), time_span_(time_span) {
  set_draw_func(sigc::mem_fun(*this, &PlotArea::on_draw));
  set_expand(true);
}

void PlotArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width,
                       int height) {
  cr->set_source_rgb(0, 0, 0);

  cr->translate(0, topgut);
  height -= topgut;
  draw_yticks(cr, height);
  width -= rightgut;
  draw_xticks(cr, width, height);

  cr->translate(xgut, 0);
  width -= xgut;
  height -= ygut;

  cr->set_line_width(box_width);
  cr->move_to(0, 0);
  cr->line_to(width, 0);
  cr->line_to(width, height);
  cr->line_to(0, height);
  cr->line_to(0, 0);
  cr->stroke();

  size_t sel_col_idx = 0;
  for (size_t i = 0; i < pin_cnt_; i++) {
    if (enabled_[i]) {
      Rgb col;
      if (sel_col_idx >= sel_colours_.size()) {
        col = next_colour();
        sel_colours_.push_back(col);
      } else {
        col = sel_colours_[sel_col_idx];
      }
      ++sel_col_idx;
      cr->set_source_rgb(col.red / 255.0f, col.green / 255.0f,
                         col.blue / 255.0f);
      draw_single_pin(data_[i], cr, width, height);
    }
  }
}

void PlotArea::draw_single_pin(const pinscope::Window &data,
                               const Cairo::RefPtr<Cairo::Context> &cr,
                               int width, int height) {
  cr->set_line_width(plot_width);

  auto it = data.begin();
  int idx = 0;
  int last_x;
  for (; it != data.end(); ++it, ++idx) {
    auto y = map_y_value(*it, height);
    if (!y)
      continue;
    int x = static_cast<float>(idx) / static_cast<float>(data.size()) * width;
    cr->move_to(x, *y);
    last_x = x;
    break;
  }

  for (; it != data.end(); ++it, ++idx) {
    int x = static_cast<float>(idx) / static_cast<float>(data.size()) * width;
    if (x == last_x)
      continue;
    last_x = x;
    auto y = map_y_value(*it, height);
    if (!y)
      continue;
    cr->line_to(x, *y);
  }
  cr->stroke();
}

void PlotArea::draw_yticks(const Cairo::RefPtr<Cairo::Context> &cr,
                           int height) {
  double max_y = height - ygut;

  for (size_t i = 0; i < yticks.size(); i++) {
    float tick_val = yticks[i];
    float tick_pos = yticks_pos[i];
    double tick_y = max_y * tick_pos;

    cr->set_line_width(ytick_width);
    cr->move_to(xgut, tick_y);
    cr->line_to(xgut - ytick_len, tick_y);
    cr->stroke();

    // Draw only whole number labels
    if (i % 2 == 1)
      continue;

    Pango::FontDescription font;
    font.set_family("Monospace");
    font.set_weight(Pango::Weight::BOLD);
    font.set_style(Pango::Style::NORMAL);
    font.set_stretch(Pango::Stretch::EXPANDED);

    auto tick_label = std::to_string(static_cast<int>(tick_val));
    auto layout = create_pango_layout(tick_label);
    layout->set_font_description(font);

    int label_width, label_height;
    layout->get_pixel_size(label_width, label_height);

    cr->move_to(xgut - (ytick_len + ytick_pad) - label_width,
                tick_y - label_height / 2.0);
    layout->show_in_cairo_context(cr);
  }
}

void PlotArea::draw_xticks(const Cairo::RefPtr<Cairo::Context> &cr, int width,
                           int height) {
  auto tick_gap = *time_span_ / (xtick_cnt - 1);
  double tick_val = 0;
  for (int i = 0; i < xtick_cnt; i++, tick_val += tick_gap) {
    double tick_x =
        xgut + i * (width - xgut) / static_cast<double>(xtick_cnt - 1);

    cr->set_line_width(xtick_width);
    cr->move_to(tick_x, height - ygut);
    cr->line_to(tick_x, height - ygut + xtick_len);
    cr->stroke();

    if (i % 2 == 1)
      continue;

    Pango::FontDescription font;
    font.set_family("Monospace");
    font.set_weight(Pango::Weight::BOLD);
    font.set_style(Pango::Style::NORMAL);
    font.set_stretch(Pango::Stretch::EXPANDED);

    auto tick_label = std::to_string(static_cast<int>(tick_val));
    auto layout = create_pango_layout(tick_label);
    layout->set_font_description(font);

    int label_width, label_height;
    layout->get_pixel_size(label_width, label_height);

    cr->move_to(tick_x - label_width / 2.0,
                height - ygut + xtick_len + xtick_pad);
    layout->show_in_cairo_context(cr);
  }
}

std::optional<int> PlotArea::map_y_value(float val, int height) const {
  if (val < min_value || val > max_value)
    return {};
  auto frac = (val - min_value) / (max_value - min_value);
  return height - static_cast<int>(height * frac);
}

static Rgb parse_colour(std::string_view str) {
  auto ptr = str.data();
  Rgb colour;
  std::from_chars(ptr + 1, ptr + 3, colour.red, 16);
  std::from_chars(ptr + 3, ptr + 5, colour.green, 16);
  std::from_chars(ptr + 5, ptr + 7, colour.blue, 16);
  return colour;
}

Rgb PlotArea::next_colour() {
  float max_diff = 0;
  std::optional<size_t> sel_col_idx;
  std::optional<Rgb> sel_col;
  for (size_t i = 0; i < colour_cnt; i++) {
    if (colour_use_[i] >= min_col_use_)
      continue;
    auto colour = parse_colour(colours[i]);
    float curr_min_diff = std::numeric_limits<float>().max();
    for (const auto &ot : sel_colours_) {
      auto diff = colour.distance(ot);
      curr_min_diff = std::min(curr_min_diff, diff);
    }
    if (curr_min_diff > max_diff) {
      max_diff = curr_min_diff;
      sel_col_idx = i;
      sel_col = colour;
    }
  }
  if (sel_col) {
    ++colour_use_[*sel_col_idx];
    return *sel_col;
  } else {
    ++min_col_use_;
    ++colour_use_[0];
    return parse_colour(colours[0]);
  }
}

} // namespace pinscope
