#include "testutils.h"
#include "lines.h"
#include "catch.hpp"
#include <string.h>


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
    const char* expected = "Hi Hello World!";
    REQUIRE(lines[0].get() == expected);
    lines.insert({(int)strlen(expected), 0}, '\n');
    REQUIRE(2 == lines.size());
    REQUIRE(lines[0].get() == expected);
    REQUIRE(lines[1].get() == "");
    lines.insert({0, 1}, "Testing\nTest");
    REQUIRE(3 == lines.size());
    REQUIRE(lines[1].get() == "Testing");
    REQUIRE(lines[2].get() == "Test");
}

TEST_CASE("Lines::remove") {
    Lines lines;
    lines.stopCapture();
    lines.insert({0, 0}, "Hi Hello World!\nTesting\nTest1\nTest2");
    REQUIRE(4 == lines.size());
    SECTION("part of a line") {
        lines.remove({3, 0}, {8, 0});
        REQUIRE(4 == lines.size());
        REQUIRE(lines[0].get() == "Hi World!");
    }
    SECTION("one full line") {
        lines.remove({0, 0}, {14, 0});
        REQUIRE(4 == lines.size());
        REQUIRE(lines[0].get() == "");
    }
    SECTION("one full line + newline") {
        lines.remove({0, 0}, {15, 0});
        REQUIRE(3 == lines.size());
        REQUIRE(lines[0].get() == "Testing");
    }
    SECTION("one full line + partial next line") {
        lines.remove({0, 0}, {0, 1});
        REQUIRE(3 == lines.size());
        REQUIRE(lines[0].get() == "esting");
    }
    SECTION("one char") {
        lines.remove({0, 0});
        REQUIRE(4 == lines.size());
        REQUIRE(lines[0].get() == "i Hello World!");
    }
}

TEST_CASE("Lines::undo-redo") {
    Lines lines;
    lines.insert({0, 0}, "Hi Hello World!\nTesting\nTest1\nTest2");
    REQUIRE(4 == lines.size());
    REQUIRE(lines.undo());
    REQUIRE_FALSE(lines.undo());
    REQUIRE(1 == lines.size());
    REQUIRE(lines[0].get() == "");
    REQUIRE(lines.redo());
    REQUIRE_FALSE(lines.redo());
    REQUIRE(4 == lines.size());
    REQUIRE(lines[1].get() == "Testing");
}

} // end namespace teditor
