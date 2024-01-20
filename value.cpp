#include "value.hpp"

namespace pinscope {

void Value::set(float val) {
  std::lock_guard<std::mutex> guard(mtx_);
  value_ = val;
}

float Value::get() const {
  std::lock_guard<std::mutex> guard(mtx_);
  return value_;
}

} // namespace pinscope
