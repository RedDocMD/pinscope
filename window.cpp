#include "window.hpp"

namespace pinscope {

Window::Window(size_t win_cap) : win_cap_(win_cap) {}

void Window::push_back(float val) {
  if (win_.size() == win_cap_) {
    win_.pop_front();
  }
  win_.push_back(val);
}

void Window::resize(size_t new_win_cap) {
  std::deque<float> new_win;
  for (auto it = win_.rbegin(); it != win_.rend(); ++it) {
    if (new_win.size() >= new_win_cap)
      break;
    new_win.push_front(*it);
  }
  win_ = std::move(new_win);
}

} // namespace pinscope
