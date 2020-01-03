#pragma once

namespace teditor {

/**
 * @brief A simple double buffer container for ping-pong kind of operations
 * @tparam Type underlying data-type
 */
template <typename Type>
struct DoubleBuffer {
  DoubleBuffer(): data(), pos(0) {}
  Type& current() { return data[pos]; }
  Type& next() { return data[pos ^ 1]; }
  void update() { pos ^= 1; }

 private:
  Type data[2];
  int pos;
};  // struct DoubleBuffer

}  // namespace teditor
