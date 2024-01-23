#pragma once

#include <mutex>

namespace pinscope {

template <typename T> class Value {
public:
  Value() = default;
  Value(Value &) = delete;
  Value &operator=(Value &) = delete;

  void set(T val) {
    std::lock_guard<std::mutex> guard(mtx_);
    value_ = val;
  }

  T get() const {
    std::lock_guard<std::mutex> guard(mtx_);
    return value_;
  }

private:
  mutable std::mutex mtx_;
  T value_;
};

} // namespace pinscope
