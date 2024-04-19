#pragma once

#include <cstddef>
#include <deque>

namespace pinscope {

class Window {
public:
  Window(size_t win_cap = 0);
  void push_back(float val);
  void resize(size_t new_win_cap);
  size_t size() const { return win_.size(); }

  using const_iterator = std::deque<float>::const_iterator;
  const_iterator begin() const { return win_.begin(); }
  const_iterator end() const { return win_.end(); }

private:
  size_t win_cap_;
  std::deque<float> win_{};
};

} // namespace pinscope
