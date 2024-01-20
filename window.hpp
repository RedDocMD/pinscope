#pragma once

#include <deque>

namespace pinscope {

class Window {
public:
  Window(size_t win_cap = 0);
  void push_back(float val);
  void resize(size_t new_win_cap);

private:
  size_t win_cap_;
  std::deque<float> win_{};
};

} // namespace pinscope
