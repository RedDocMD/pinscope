#include "pinscope.hpp"
#include <sstream>

namespace pinscope {

Pinscope::Pinscope() {
  constexpr int default_width = 500;
  constexpr int default_height = 500;

  set_title("PinScope");
  set_default_size(default_width, default_height);

  for (int i = 0; i < pin_cnt; i++) {
    std::stringstream ss;
    ss << "Pin " << i;
    pin_cbox_[i].set_label(ss.str());
    pin_cbox_[i].signal_toggled().connect(
        sigc::bind(sigc::mem_fun(*this, &Pinscope::on_pin_cbox_toggled), i));
  }

  for (int i = 0; i < pin_cnt / pin_per_row; i++) {
    for (int j = 0; j < pin_per_row; j++) {
      pin_grid_.attach(pin_cbox_[i * pin_per_row + j], j, i);
    }
  }
  pin_grid_.set_margin(10);
  set_child(pin_grid_);
}

void Pinscope::on_pin_cbox_toggled(int idx) {
  pin_enable_[idx] = pin_cbox_[idx].get_active();
}

} // namespace pinscope
