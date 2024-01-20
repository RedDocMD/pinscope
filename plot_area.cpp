#include "plot_area.hpp"
#include <iostream>

namespace pinscope {

PlotArea::PlotArea(const pinscope::Window *data, const bool *enabled,
                   size_t pin_cnt)
    : data_(data), enabled_(enabled), pin_cnt_(pin_cnt) {
  set_draw_func(sigc::mem_fun(*this, &PlotArea::on_draw));
  set_expand(true);
}

void PlotArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width,
                       int height) const {
  for (size_t i = 0; i < pin_cnt_; i++) {
    if (enabled_[i]) {
      cr->set_source_rgb(0, 0, 0);
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
  for (;; ++it, ++idx) {
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

} // namespace pinscope
