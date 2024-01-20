#include "pinscope.hpp"
#include <gtkmm/adjustment.h>
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

  auto time_span_sb_adj = time_span_sb_.get_adjustment().get();
  time_span_ = default_time_span;
  time_span_sb_adj->set_value(time_span_);
  time_span_sb_adj->set_lower(0);
  time_span_sb_adj->set_upper(max_time_span);
  time_span_sb_adj->set_step_increment(1);
  time_span_sb_adj->set_page_increment(10);
  time_span_sb_.set_digits(0);
  time_span_sb_.set_snap_to_ticks(true);
  time_span_sb_.set_value(time_span_);
  time_span_sb_.set_numeric(true);
  time_span_sb_.signal_value_changed().connect(
      sigc::mem_fun(*this, &Pinscope::on_time_span_sb_set));

  time_span_label_.set_label("Time span (ms)");
  time_span_box_.set_orientation(Gtk::Orientation::HORIZONTAL);
  time_span_box_.append(time_span_label_);
  time_span_box_.append(time_span_sb_);
  time_span_box_.set_spacing(10);
  time_span_box_.set_margin(10);

  main_box_.set_orientation(Gtk::Orientation::VERTICAL);
  main_box_.append(time_span_box_);
  main_box_.append(pin_grid_);

  set_child(main_box_);
}

void Pinscope::on_pin_cbox_toggled(int idx) {
  pin_enable_[idx] = pin_cbox_[idx].get_active();
}

void Pinscope::on_time_span_sb_set() {
  time_span_ = time_span_sb_.get_value_as_int();
  auto win_size = time_span_ / time_resolution;
  for (auto &pin_data : data_) {
    pin_data.resize(win_size);
  }
}

} // namespace pinscope
