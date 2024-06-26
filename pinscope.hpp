#pragma once

#include "plot_area.hpp"
#include "value.hpp"
#include "window.hpp"
#include <array>
#include <gtkmm.h>
#include <optional>
#include <thread>

namespace pinscope {

class Pinscope : public Gtk::Window {
public:
  Pinscope();
  ~Pinscope() override = default;

private:
  static constexpr int pin_cnt = 27;
  static constexpr int pin_per_row = 9;
  static constexpr int default_time_span = 1000;
  static constexpr int max_time_span = 10000;
  static constexpr int time_resolution = 1;

  std::array<pinscope::Window, pin_cnt> data_;
  std::array<pinscope::Value<float>, pin_cnt> level_;
  std::array<bool, pin_cnt> pin_enable_;
  int time_span_;
  std::thread sock_thread_;
  pinscope::Value<bool> quit_sock_;

  PlotArea plot_;
  std::array<Gtk::CheckButton, pin_cnt> pin_cbox_;
  Gtk::Label time_span_label_;
  Gtk::SpinButton time_span_sb_;
  Gtk::Box time_span_box_;
  Gtk::Grid pin_grid_;
  Gtk::Box main_box_;

  void on_pin_cbox_toggled(int idx);
  void on_time_span_sb_set();
  bool on_timer_step();
  bool on_quit();

  void update_levels();
  std::optional<int> map_portpin_to_pin(int port, int pin);
};

} // namespace pinscope
