#include "pos2d.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("Pos2d") {
    Pos2d<int> a = {10, 10};
    Pos2d<int> b = {0, 0};
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
    Pos2d<int> a = {10, 10};
    Pos2d<int> b = {0, 0};
    Pos2d<int> start, end;
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

TEST_CASE("Positions::==") {
    Positions a = {
        {10, 10},
        {3, 2}
    };
    Positions b;
    REQUIRE(a == a);
    REQUIRE_FALSE(a == b);
    REQUIRE_FALSE(b == a);
    b = {
        {2, 3}
    };
    REQUIRE_FALSE(b == a);
    b.push_back({1, 2});
    REQUIRE_FALSE(b == a);
}

TEST_CASE("Positions::added") {
    Positions a = {
        {2, 3},
        {8, 3},
        {6, 4}
    };
    Strings chars = {"abc\ndef", "ghij\nkl", "mno"};
    a.added(chars);
    Positions expected = {
        {3, 4},
        {2, 5},
        {9, 6}
    };
    REQUIRE(a == expected);
}

} // end namespace teditor
