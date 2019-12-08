#include "extensions/calc/number.h"
#include "catch.hpp"
#include "testutils.h"

namespace teditor {
namespace calc {

TEST_CASE("Number") {
  SECTION("default ctor") {
    Num32 n;
    REQUIRE(n.isInt);
    REQUIRE(n.i == 0);
  }
  SECTION("ctor int") {
    Num32 n(1);
    REQUIRE(n.isInt);
    REQUIRE(n.i == 1);
  }
  SECTION("ctor float") {
    Num32 n(1.5f);
    REQUIRE_FALSE(n.isInt);
    REQUIRE(n.f == 1.5f);
  }
  SECTION("copy ctor") {
    Num32 n1(2), n2(2.5f);
    Num32 m1(n1), m2(n2);
    REQUIRE(m1.isInt);
    REQUIRE(m1.i == 2);
    REQUIRE_FALSE(m2.isInt);
    REQUIRE(m2.f == 2.5f);
  }
  SECTION("copy") {
    Num32 m = Num32(3);
    REQUIRE(m.isInt);
    REQUIRE(m.i == 3);
    m = Num32(3.5f);
    REQUIRE_FALSE(m.isInt);
    REQUIRE(m.f == 3.5f);
  }
}

}  // namespace calc
}  // namespace teditor
