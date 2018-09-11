#include "mode.h"
#include "catch.hpp"

namespace teditor {

TEST_CASE("Mode::createMode") {
    REQUIRE_THROWS(Mode::createMode("not-exist"));
    REQUIRE("text" == Mode::createMode("text")->name());
}

TEST_CASE("Mode::inferMode") {
    REQUIRE("text" == Mode::inferMode("no-match"));
    REQUIRE("dir" == Mode::inferMode("src"));
    REQUIRE("text" == Mode::inferMode("file.txt"));
}

} // end namespace teditor
