#include "plot_area.hpp"
#include <charconv>

namespace pinscope {

PlotArea::PlotArea(const pinscope::Window *data, const bool *enabled,
                   size_t pin_cnt)
    : data_(data), enabled_(enabled), pin_cnt_(pin_cnt) {
  set_draw_func(sigc::mem_fun(*this, &PlotArea::on_draw));
  set_expand(true);
}

void PlotArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width,
                       int height) const {
  cr->set_source_rgb(0, 0, 0);
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
                               int width, int height) const {
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

Rgb PlotArea::next_colour() const {
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
