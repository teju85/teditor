#pragma once

#include <stdint.h>

namespace teditor {
namespace calc {

#define OP(b, op) do {                \
    if (isInt && b.isInt) {           \
      i = i op b.i;                   \
    } else if (isInt && !b.isInt) {   \
      isInt = false;                  \
      f = FloatT(i) op b.f;           \
    } else if (!isInt && b.isInt) {   \
      f = f op FloatT(b.i);           \
    } else {                          \
      f = f op b.f;                   \
    }                                 \
  } while(0);                         \
  return *this

#define B_OP(b, op) do {                                \
    if (isInt && b.isInt) return i op b.i;              \
    if (isInt && !b.isInt) return FloatT(i) op b.f;     \
    if (!isInt && b.isInt) return f op FloatT(b.i);     \
    return f op b.f;                                    \
  } while(0)

/**
 * @brief Core number used for computing in the calculator
 * @tparam IntT integer type
 * @tparam FloatT float type
 */
template <typename IntT, typename FloatT>
struct Number {
  typedef Number<IntT, FloatT> Num;

  /** actual value stored */
  union {
    IntT i;
    FloatT f;
  };  // union
  /** whether the storage is currently via int or float */
  bool isInt;

  Number() : i(0), isInt(true) {}
  Number(IntT v) : i(v), isInt(true) {}
  Number(FloatT v) : f(v), isInt(false) {}
  Number(const Num& n) : i(n.i), isInt(n.isInt) {}

  const Num& operator=(const Num& n) {
    i = n.i;
    isInt = n.isInt;
    return *this;
  }

  Num& operator+() { return *this; }
  Num& operator-() {
    if (isInt) i = -i;
    else f = -f;
    return *this;
  }

  // inplace operators
  const Num& operator+=(const Num& b) { OP(b, +); }
  const Num& operator-=(const Num& b) { OP(b, -); }
  const Num& operator*=(const Num& b) { OP(b, *); }
  const Num& operator/=(const Num& b) { OP(b, /); }

  // comparisons
  bool operator==(const Num& b) const { B_OP(b, ==); }
  bool operator!=(const Num& b) const { B_OP(b, !=); }
  bool operator<(const Num& b) const { B_OP(b, <); }
  bool operator<=(const Num& b) const { B_OP(b, <=); }
  bool operator>(const Num& b) const { B_OP(b, >); }
  bool operator>=(const Num& b) const { B_OP(b, >=); }

  // conversions
  Num toInt() const {
    Num ret(*this);
    if (!ret.isInt) {
      ret.isInt = true;
      ret.i = IntT(ret.f);
    }
    return ret;
  }
  Num toFloat() const {
    Num ret(*this);
    if (ret.isInt) {
      ret.isInt = false;
      ret.f = FloatT(ret.i);
    }
    return ret;
  }
};  // struct Number


// operator overloads
template <typename I, typename F>
Number<I, F> operator+(const Number<I, F>& a, const Number<I, F>& b) {
  Number<I, F> ret = a;
  ret += b;
  return ret;
}
template <typename I, typename F>
Number<I, F> operator-(const Number<I, F>& a, const Number<I, F>& b) {
  Number<I, F> ret = a;
  ret -= b;
  return ret;
}
template <typename I, typename F>
Number<I, F> operator*(const Number<I, F>& a, const Number<I, F>& b) {
  Number<I, F> ret = a;
  ret *= b;
  return ret;
}
template <typename I, typename F>
Number<I, F> operator/(const Number<I, F>& a, const Number<I, F>& b) {
  Number<I, F> ret = a;
  ret /= b;
  return ret;
}

#undef B_OP
#undef OP

typedef Number<int32_t, float> Num32;
typedef Number<int64_t, double> Num64;

}  // namespace calc
}  // namespace teditor
