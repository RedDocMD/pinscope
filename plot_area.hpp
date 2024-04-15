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
  PlotArea(const pinscope::Window *data, const bool *enabled, size_t pin_cnt,
           const int *time_span);

private:
  constexpr static float min_value = 0.0f;
  constexpr static float max_value = 5.0f;
  constexpr static size_t colour_cnt = 14;
  constexpr static std::array<std::string_view, colour_cnt> colours = {
      "#ec4899", "#f43f5e", "#f87171", "#fb923c", "#fbbf24",
      "#a3e635", "#34d399", "#2dd4bf", "#22d3ee", "#38bdf8",
      "#60a5fa", "#818cf8", "#a78bfa", "#e879f9",
  };

  constexpr static double box_width = 1.0;

  constexpr static int xgut = 40;
  constexpr static int ygut = 40;
  constexpr static int topgut = 10;
  constexpr static int rightgut = 20;

  constexpr static double ytick_len = 5.0;
  constexpr static double ytick_width = 1.5;
  constexpr static double ytick_pad = 2.0;

  constexpr static std::array<float, 11> yticks = {
      0.0f, 0.5f, 1.0f, 1.5f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f, 4.5f, 5.0f};
  constexpr static std::array<float, 11> yticks_pos = {
      1.0f, 0.9f, 0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f, 0.0f};

  constexpr static double xtick_len = 5.0;
  constexpr static double xtick_width = 1.5;
  constexpr static double xtick_pad = 2.0;
  constexpr static int xtick_cnt = 11;

  constexpr static double plot_width = 3;

  void on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width, int height);

  void draw_yticks(const Cairo::RefPtr<Cairo::Context> &cr, int height);
  void draw_xticks(const Cairo::RefPtr<Cairo::Context> &cr, int width,
                   int height);
  void draw_single_pin(const pinscope::Window &data,
                       const Cairo::RefPtr<Cairo::Context> &cr, int width,
                       int height);
  std::optional<int> map_y_value(float val, int height) const;

  Rgb next_colour();

  const pinscope::Window *data_;
  const bool *enabled_;
  size_t pin_cnt_;
  const int *time_span_;

  std::array<size_t, colour_cnt> colour_use_{0};
  std::vector<Rgb> sel_colours_{};
  size_t min_col_use_{0};
};

} // namespace pinscope
