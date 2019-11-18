#include "core/pos2d.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("Pos2d") {
  Point a = {10, 10};
  Point b = {0, 0};
  REQUIRE(a > b);
  REQUIRE(a >= b);
  REQUIRE_FALSE(a == b);
  REQUIRE(a != b);
  REQUIRE_FALSE(a <= b);
  REQUIRE_FALSE(a < b);
  b = {10, 10};
  REQUIRE_FALSE(a > b);
  REQUIRE(a >= b);
  REQUIRE(a == b);
  REQUIRE_FALSE(a != b);
  REQUIRE(a <= b);
  REQUIRE_FALSE(a < b);
  b = {11, 10};
  REQUIRE_FALSE(a > b);
  REQUIRE_FALSE(a >= b);
  REQUIRE_FALSE(a == b);
  REQUIRE(a != b);
  REQUIRE(a <= b);
  REQUIRE(a < b);
  b = {9, 11};
  REQUIRE_FALSE(a > b);
  REQUIRE_FALSE(a >= b);
  REQUIRE_FALSE(a == b);
  REQUIRE(a != b);
  REQUIRE(a <= b);
  REQUIRE(a < b);
  a = {0, 0};
  a += "testing";
  REQUIRE(a.x == 7);  REQUIRE(a.y == 0);
  a += "\ntesting";
  REQUIRE(a.x == 7);  REQUIRE(a.y == 1);
}

TEST_CASE("Pos2d::find") {
  Point a = {10, 10};
  Point b = {0, 0};
  Point start, end;
  REQUIRE(1 == a.find(start, end, b));
  REQUIRE(start == b);
  REQUIRE(end == a);
  b = {10, 10};
  REQUIRE(0 == a.find(start, end, b));
  REQUIRE(start == b);
  REQUIRE(end == a);
  b = {8, 10};
  REQUIRE(0 == a.find(start, end, b));
  REQUIRE(start == b);
  REQUIRE(end == a);
  b = {11, 10};
  REQUIRE(0 == a.find(start, end, b));
  REQUIRE(start == a);
  REQUIRE(end == b);
  b = {9, 11};
  REQUIRE(-1 == a.find(start, end, b));
  REQUIRE(start == a);
  REQUIRE(end == b);
}

TEST_CASE("Pos2d::isInside") {
  Point cu(2, 3);
  Point reg(-1, -1);
  REQUIRE_FALSE(reg.isInside(3, 4, cu));
  reg = {5, 3};
  REQUIRE(reg.isInside(3, 4, cu));
}

} // end namespace teditor
