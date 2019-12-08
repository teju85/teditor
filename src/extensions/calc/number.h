#pragma once

#include <stdint.h>

namespace teditor {
namespace calc {

#define OP(a, b, op) do {                       \
    if (a.isInt && b.isInt) {                   \
      a.i = a.i op b.i;                         \
    } else if (a.isInt && !b.isInt) {           \
      a.isInt = false;                          \
      a.f = FloatT(a.i) op b.f;                 \
    } else if (!a.isInt && b.isInt) {            \
      a.f = a.f op FloatT(b.i);                 \
    } else {                                    \
      a.f = a.f op b.f;                         \
    }                                           \
  } while(0)

#define B_OP(a, b, op) do {                             \
    if (a.isInt && b.isInt) return a.i op b.i;          \
    if (a.isInt && !b.isInt) return FloatT(a.i) op b.f; \
    if (!a.isInt && b.isInt) return a.f op Float(b.i);  \
    return a.f op b.f;                                  \
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
  const Num& operator+=(const Num& b) {
    OP(*this, b, +);
    return *this;
  }
  const Num& operator-=(const Num& b) {
    OP(*this, b, -);
    return *this;
  }
  const Num& operator*=(const Num& b) {
    OP(*this, b, *);
    return *this;
  }
  const Num& operator/=(const Num& b) {
    OP(*this, b, /);
    return *this;
  }

  // comparisons
  bool operator==(const Num& b) const { return i == b.i; }
  bool operator!=(const Num& b) const { return i != b.i; }
  bool operator<(const Num& b) const { B_OP(*this, b, <); }
  bool operator<=(const Num& b) const { B_OP(*this, b, <=); }
  bool operator>(const Num& b) const { B_OP(*this, b, >); }
  bool operator>=(const Num& b) const { B_OP(*this, b, >=); }

  // conversions
  Num toInt() const {
    Num ret(*this);
    if (!ret.isInt) {
      ret.isInt = true;
      ret.f = IntT(ret.i);
    }
    return ret;
  }
  Num toFloat() const {
    Num ret(*this);
    if (ret.isInt) {
      ret.isInt = false;
      ret.i = Float(ret.f);
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
