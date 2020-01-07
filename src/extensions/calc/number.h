#pragma once

#include <stdint.h>
#include <cmath>

namespace std {
template <typename T> T sq(T in) { return in * in; }
template <typename T> T cube(T in) { return in * in * in; }
};  // namespace std

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

  static const Num e;
  static const Num pi;
  static const Num nan;
  static const Num log2e;
  static const Num log10e;
  static const Num sqrt2;
  static const Num sqrt1_2;
};  // struct Number

#undef B_OP
#undef OP


#define CONST(var, val)                         \
  template <typename I, typename F>             \
  const Number<I, F> Number<I, F>::var((F)val)
CONST(e, M_E);
CONST(pi, M_PI);
CONST(nan, NAN);
CONST(log2e, M_LOG2E);
CONST(log10e, M_LOG10E);
CONST(sqrt2, M_SQRT2);
CONST(sqrt1_2, M_SQRT1_2);
#undef CONST


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

// generic functions
#define FUNC(name)                              \
  template <typename I, typename F>             \
  Number<I, F> name(const Number<I, F>& in) {   \
    Number<I, F> out;                           \
    out.isInt = in.isInt;                       \
    if (out.isInt) {                            \
      out.i = std::name(in.i);                  \
    } else {                                    \
      out.f = std::name(in.f);                  \
    }                                           \
    return out;                                 \
  }
FUNC(sq);
FUNC(cube);
#undef FUNC

// floating point functions
#define FP_FUNC(name)                                   \
  template <typename I, typename F>                     \
  Number<I, F> name(const Number<I, F>& in) {           \
    Number<I, F> out;                                   \
    out.isInt = false;                                  \
    out.f = std::name(in.isInt ? F(in.i) : in.f);       \
    return out;                                         \
  }
FP_FUNC(abs);
FP_FUNC(sin);
FP_FUNC(cos)
FP_FUNC(tan);
FP_FUNC(asin);
FP_FUNC(acos)
FP_FUNC(atan);
FP_FUNC(sinh);
FP_FUNC(cosh)
FP_FUNC(tanh);
FP_FUNC(asinh);
FP_FUNC(acosh)
FP_FUNC(atanh);
FP_FUNC(sqrt);
FP_FUNC(cbrt);
FP_FUNC(log);
FP_FUNC(log10);
FP_FUNC(exp);
FP_FUNC(floor);
FP_FUNC(ceil);
FP_FUNC(round);
#undef FP_FUNC

#define FP_FUNC2(name)                                                  \
  template <typename I, typename F>                                     \
  Number<I, F> name(const Number<I, F>& in1, const Number<I, F>& in2) { \
    Number<I, F> out;                                                   \
    out.isInt = false;                                                  \
    out.f = std::name(in1.isInt ? F(in1.i) : in1.f,                     \
                      in2.isInt ? F(in2.i) : in2.f);                    \
    return out;                                                         \
  }
FP_FUNC2(pow);
FP_FUNC2(hypot);
#undef FP_FUNC2

typedef Number<int32_t, float> Num32;
typedef Number<int64_t, double> Num64;

}  // namespace calc
}  // namespace teditor
