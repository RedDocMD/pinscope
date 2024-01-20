#pragma once

#include "window.hpp"
#include <gtkmm/drawingarea.h>
#include <optional>

namespace pinscope {

class PlotArea : public Gtk::DrawingArea {
public:
  PlotArea(const pinscope::Window *data, const bool *enabled, size_t pin_cnt);

private:
  constexpr static float min_value = 0.0f;
  constexpr static float max_value = 5.0f;

  void on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width,
               int height) const;
  void draw_single_pin(const pinscope::Window &data,
                       const Cairo::RefPtr<Cairo::Context> &cr, int width,
                       int height) const;
  std::optional<int> map_y_value(float val, int height) const;

  const pinscope::Window *data_;
  const bool *enabled_;
  size_t pin_cnt_;
};

} // namespace pinscope
