#pragma once

#include <mutex>

namespace pinscope {

class Value {
public:
  Value() = default;
  Value(Value &) = delete;
  Value &operator=(Value &) = delete;

  void set(float val);
  float get() const;

private:
  mutable std::mutex mtx_;
  float value_{0.0f};
};

} // namespace pinscope
