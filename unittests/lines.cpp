#include "testutils.h"
#include "lines.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("Lines::Lines") {
    Lines lines;
    REQUIRE(1 == lines.size());
    REQUIRE(lines.canCapture());
    REQUIRE_FALSE(lines.undo());
    REQUIRE_FALSE(lines.redo());
}

TEST_CASE("Lines::insert") {
    Lines lines;
    lines.stopCapture();
    REQUIRE_FALSE(lines.canCapture());
    lines.insert({0, 0}, "Hello World!");
    REQUIRE_FALSE(lines.redo());
    REQUIRE_FALSE(lines.undo());
    REQUIRE(lines[0].get() == "Hello World!");
    lines.insert({0, 0}, ' ');
    lines.insert({0, 0}, 'i');
    lines.insert({0, 0}, 'H');
    REQUIRE_FALSE(lines.redo());
    REQUIRE_FALSE(lines.undo());
    REQUIRE(lines[0].get() == "Hi Hello World!");
    lines.insert({15, 0}, '\n');
    REQUIRE(2 == lines.size());
    REQUIRE(lines[0].get() == "Hi Hello World!");
    REQUIRE(lines[1].get() == "");
    lines.insert({0, 1}, "Testing\nTest");
    REQUIRE(3 == lines.size());
    REQUIRE(lines[1].get() == "Testing");
    REQUIRE(lines[2].get() == "Test");
}

} // end namespace teditor
