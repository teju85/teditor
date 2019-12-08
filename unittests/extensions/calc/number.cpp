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
  REQUIRE(n.f == val)

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
}

#undef FLT_CHECK
#undef INT_CHECK

}  // namespace calc
}  // namespace teditor
