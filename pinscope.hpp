#pragma once

#include <array>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/window.h>

namespace pinscope {

class Pinscope : public Gtk::Window {
public:
  Pinscope();
  ~Pinscope() override = default;

private:
  static constexpr int pin_cnt = 27;
  static constexpr int pin_per_row = 9;

  std::array<Gtk::CheckButton, pin_cnt> pin_cbox_;
  std::array<bool, pin_cnt> pin_enable_;
  Gtk::Grid pin_grid_;

  void on_pin_cbox_toggled(int idx);
};

} // namespace pinscope
