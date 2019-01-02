#include "core/pos2d.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("Pos2d") {
    Pos2di a = {10, 10};
    Pos2di b = {0, 0};
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
    Pos2di a = {10, 10};
    Pos2di b = {0, 0};
    Pos2di start, end;
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
    Positions c(a);
    REQUIRE(c == a);
}

TEST_CASE("Positions::added1") {
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

TEST_CASE("Positions::added2") {
    Positions a = {
        {2, 3},
        {8, 3},
        {6, 4}
    };
    std::string chars = "abc\ndef";
    a.added(chars);
    Positions expected = {
        {3, 4},
        {3, 5},
        {3, 7}
    };
    REQUIRE(a == expected);
}

} // end namespace teditor
