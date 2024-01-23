#include "pinscope.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

namespace pinscope {

Pinscope::Pinscope() : plot_(data_.data(), pin_enable_.data(), pin_cnt) {
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
    pin_enable_[i] = false;
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

  plot_.set_margin(10);

  main_box_.set_orientation(Gtk::Orientation::VERTICAL);
  main_box_.append(plot_);
  main_box_.append(time_span_box_);
  main_box_.append(pin_grid_);

  set_child(main_box_);

  for (auto &pin_data : data_) {
    pin_data.resize(time_span_ / time_resolution);
  }

  std::thread th(&Pinscope::update_levels, this);
  sock_thread_ = std::move(th);

  Glib::signal_timeout().connect(sigc::mem_fun(*this, &Pinscope::on_timer_step),
                                 time_resolution);
}

void Pinscope::on_pin_cbox_toggled(int idx) {
  pin_enable_[idx] = pin_cbox_[idx].get_active();
  plot_.queue_draw();
}

void Pinscope::on_time_span_sb_set() {
  time_span_ = time_span_sb_.get_value_as_int();
  auto win_size = time_span_ / time_resolution;
  for (auto &pin_data : data_) {
    pin_data.resize(win_size);
  }
  plot_.queue_draw();
}

bool Pinscope::on_timer_step() {
  for (int i = 0; i < pin_cnt; i++) {
    auto pin_level = level_[i].get();
    data_[i].push_back(pin_level);
  }
  plot_.queue_draw();
  return true;
}

std::optional<int> Pinscope::map_portpin_to_pin(int port, int pin) {
  struct {
    int port;
    int pin;
  } pin_cfg[pin_cnt] = {
      {.port = 3, .pin = 1},  {.port = 3, .pin = 2},  {.port = 1, .pin = 5},
      {.port = 1, .pin = 4},  {.port = 1, .pin = 3},  {.port = 1, .pin = 2},
      {.port = 1, .pin = 6},  {.port = 1, .pin = 7},  {.port = 3, .pin = 4},
      {.port = 3, .pin = 3},  {.port = 1, .pin = 12}, {.port = 1, .pin = 9},
      {.port = 1, .pin = 10}, {.port = 1, .pin = 11}, {.port = 0, .pin = 14},
      {.port = 0, .pin = 0},  {.port = 0, .pin = 1},  {.port = 0, .pin = 2},
      {.port = 1, .pin = 1},  {.port = 1, .pin = 0},  {.port = 5, .pin = 0},
      {.port = 0, .pin = 12}, {.port = 0, .pin = 13}, {.port = 5, .pin = 1},
      {.port = 5, .pin = 2},  {.port = 1, .pin = 8},  {.port = 3, .pin = 0},
  };

  for (int i = 0; i < pin_cnt; i++) {
    if (pin_cfg[i].pin == pin && pin_cfg[i].port == port) {
      return i;
    }
  }
  return {};
}

extern "C" struct pin_value {
  uint32_t port;
  uint32_t pin;
  float value;
};

struct MsgException : std::exception {
  std::string msg;

  MsgException(const std::string &msg) : msg(msg) {}

  const char *what() const noexcept override { return msg.c_str(); }
};

void Pinscope::update_levels() {
  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    throw MsgException("Failed to create socket");
  }

  sockaddr_in addr;
  constexpr uint16_t port = 3500;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  inet_pton(AF_INET, "localhost", &addr.sin_addr);
  addr.sin_port = htons(port);

  while (true) {
    if (connect(sockfd, reinterpret_cast<const sockaddr *>(&addr),
                sizeof(addr)) >= 0) {
      break;
    }
    sleep(1);
  }

  pin_value pv;
  while (true) {
    auto cnt = read(sockfd, &pv, sizeof(pv));
    if (cnt < 0) {
      throw MsgException("Read error");
    }
    if (static_cast<size_t>(cnt) < sizeof(pv)) {
      std::cerr << "Read less bytes than expected\n";
      continue;
    }
    auto actual_pin = map_portpin_to_pin(pv.port, pv.pin);
    if (!actual_pin) {
      std::cerr << "Received invalid pin\n";
      continue;
    }
    level_[*actual_pin].set(pv.value);
  }
}

} // namespace pinscope
