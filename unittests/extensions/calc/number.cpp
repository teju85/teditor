#include "extensions/calc/number.h"
#include "catch.hpp"
#include "testutils.h"

namespace teditor {
namespace calc {

#define INT_CHECK(n, val)                       \
  REQUIRE(n.isInt);                             \
  REQUIRE(n.i == val)

#define FLT_CHECK(n, val)                       \
  REQUIRE_FALSE(n.isInt);                       \
  REQUIRE(n.f == Approx(val))

// used mainly for testing of catching exceptions
Num32 createNum(const std::string& str) { return Num32(str); }

TEST_CASE("Number") {
  SECTION("default ctor") {
    Num32 n;
    INT_CHECK(n, 0);
  }
  SECTION("ctor int") {
    Num32 n(1);
    INT_CHECK(n, 1);
  }
  SECTION("ctor float") {
    Num32 n(1.5f);
    FLT_CHECK(n, 1.5f);
  }
  SECTION("copy ctor") {
    Num32 n1(2), n2(2.5f);
    Num32 m1(n1), m2(n2);
    INT_CHECK(m1, 2);
    FLT_CHECK(m2, 2.5f);
  }
  SECTION("copy") {
    Num32 m = Num32(3);
    INT_CHECK(m, 3);
    m = Num32(3.5f);
    FLT_CHECK(m, 3.5f);
    m = 4;
    INT_CHECK(m, 4);
    m = 4.5f;
    FLT_CHECK(m, 4.5f);
  }
  SECTION("unary+-") {
    Num32 n(2);
    INT_CHECK(n, 2);
    n = +n;
    INT_CHECK(n, 2);
    n = -n;
    INT_CHECK(n, -2);
  }
  SECTION("+=") {  // skipping -=, *= and /= tests as they are all similar
    Num32 n1(3), n2(3.5f);
    Num32 m11(2), m12(2.5f), m21(2), m22(2.5f);
    m11 += n1;
    INT_CHECK(m11, 5);
    m12 += n1;
    FLT_CHECK(m12, 5.5f);
    m21 += n2;
    FLT_CHECK(m21, 5.5f);
    m22 += n2;
    FLT_CHECK(m22, 6.0f);
  }
  SECTION("==") {  // skipping != as it is similar
    Num32 n1(2), n2(2.0f), n3;
    REQUIRE_FALSE(n1 == n3);
    REQUIRE(n1 == n2);
    n3 = 2;
    REQUIRE(n1 == n3);
  }
  SECTION("<") {  // skipping >=, < and <= tests as they are all similar
    Num32 n1(2), n2(2.0f), n3;
    REQUIRE_FALSE(n1 < n2);
    REQUIRE_FALSE(n2 < n1);
    REQUIRE_FALSE(n1 < n3);
    REQUIRE(n3 < n1);
  }
  SECTION("toInt") {
    Num32 n(2.5f), p(3);
    auto m = n.toInt();
    INT_CHECK(m, 2);
    m = p.toInt();
    INT_CHECK(m, 3);
  }
  SECTION("toFloat") {
    Num32 n(2.5f), p(3);
    auto m = n.toFloat();
    FLT_CHECK(m, 2.5f);
    m = p.toFloat();
    FLT_CHECK(m, 3.f);
  }
  SECTION("+") {  // skipping -, * and / tests as they are all similar
    Num32 n1(1), n2(1.5f);
    auto m = n1 + n1;
    INT_CHECK(m, 2);
    m = n1 + n2;
    FLT_CHECK(m, 2.5f);
    m = n2 + n1;
    FLT_CHECK(m, 2.5f);
    m = n2 + n2;
    FLT_CHECK(m, 3.0f);
  }
  SECTION("sine and arcsine") {
    Num32 n(0);
    auto m = sin(n);
    FLT_CHECK(m, 0.f);
    m = asin(n);
    FLT_CHECK(m, 0.f);
  }
  SECTION("log and log10") {
    Num32 n(1.f);
    auto m = log(n);
    FLT_CHECK(m, 0.f);
    m = log10(n);
    FLT_CHECK(m, 0.f);
  }
  SECTION("sqrt and cbrt") {
    Num32 n(1.f);
    auto m = sqrt(n);
    FLT_CHECK(m, 1.f);
    m = cbrt(n);
    FLT_CHECK(m, 1.f);
  }
  SECTION("exp") {
    Num32 n(0.f);
    auto m = exp(n);
    FLT_CHECK(m, 1.f);
  }
  SECTION("e and pi") {
    Num32 n(Num32::e);
    FLT_CHECK(n, 2.71828f);
    n = Num32::pi;
    FLT_CHECK(n, 3.14159f);
  }
  SECTION("floor and ceil and round") {
    Num32 n1(1.2f), n2(1.8f);
    FLT_CHECK(floor(n1), 1.f);
    FLT_CHECK(ceil(n1), 2.f);
    FLT_CHECK(round(n1), 1.f);
    FLT_CHECK(round(n2), 2.f);
  }
  SECTION("pow") {
    Num32 n1(1.2f), n2(2.f);
    FLT_CHECK(pow(n1, n2), 1.44f);
  }
  SECTION("hypot") {
    Num32 n1(3.f), n2(4.f);
    FLT_CHECK(hypot(n1, n2), 5.f);
  }
  SECTION("nan") {
    REQUIRE(std::isnan(Num32::nan.f));
  }
  SECTION("sq and cube") {
    Num32 n(2.f);
    FLT_CHECK(sq(n), 4.f);
    FLT_CHECK(cube(n), 8.f);
  }
  SECTION("str-to-int") {
    Num32 n1("123"), n2("-123");
    INT_CHECK(n1, 123);
    INT_CHECK(n2, -123);
  }
  SECTION("str-to-float") {
    Num32 n1("123.4"), n2("-123.4");
    FLT_CHECK(n1, 123.4f);
    FLT_CHECK(n2, -123.4f);
  }
  SECTION("incorrect number string") {
    REQUIRE_THROWS(createNum("hello"));
  }
}

#undef FLT_CHECK
#undef INT_CHECK

}  // namespace calc
}  // namespace teditor
