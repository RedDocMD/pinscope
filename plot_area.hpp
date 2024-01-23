#pragma once

#include "window.hpp"
#include <array>
#include <cmath>
#include <gtkmm/drawingarea.h>
#include <optional>
#include <string_view>
#include <vector>

namespace pinscope {

struct Rgb {
  uint8_t red;
  uint8_t green;
  uint8_t blue;

  Rgb() = default;
  Rgb(uint8_t red, uint8_t green, uint8_t blue)
      : red(red), green(green), blue(blue) {}

  float distance(const Rgb &ot) const {
    auto rdiff = abs(red - ot.red);
    auto gdiff = abs(green - ot.green);
    auto bdiff = abs(blue - ot.blue);
    return sqrtf(rdiff * rdiff + gdiff * gdiff + bdiff * bdiff);
  }
};

class PlotArea : public Gtk::DrawingArea {
public:
  PlotArea(const pinscope::Window *data, const bool *enabled, size_t pin_cnt);

private:
  constexpr static float min_value = 0.0f;
  constexpr static float max_value = 5.0f;
  constexpr static size_t colour_cnt = 14;
  constexpr static std::array<std::string_view, colour_cnt> colours = {
      "#ec4899", "#f43f5e", "#f87171", "#fb923c", "#fbbf24",
      "#a3e635", "#34d399", "#2dd4bf", "#22d3ee", "#38bdf8",
      "#60a5fa", "#818cf8", "#a78bfa", "#e879f9",
  };

  void on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width,
               int height) const;
  void draw_single_pin(const pinscope::Window &data,
                       const Cairo::RefPtr<Cairo::Context> &cr, int width,
                       int height) const;
  std::optional<int> map_y_value(float val, int height) const;

  Rgb next_colour() const;

  const pinscope::Window *data_;
  const bool *enabled_;
  size_t pin_cnt_;

  mutable std::array<size_t, colour_cnt> colour_use_{0};
  mutable std::vector<Rgb> sel_colours_{};
  mutable size_t min_col_use_{0};
};

} // namespace pinscope
