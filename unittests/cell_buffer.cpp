#include "core/cell_buffer.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("CellBuffer::Cell") {
    Cell c = {0, 0, 0};
    REQUIRE(0 == c.width());
    c.ch = 'c';
    REQUIRE(1 == c.width());
    Cell b;
    b.copy(c);
    REQUIRE(c == b);
    b.set('b', 0, 0);
    REQUIRE_FALSE(b == c);
}

TEST_CASE("CellBuffer::CellBuffer") {
    AttrColor fg=0, bg=1;
    CellBuffer cb(5, 5);
    cb.clear(fg, bg);
    Cell clr = {' ', fg, bg};
    REQUIRE(clr == cb.at(0, 0));
    REQUIRE(clr == cb.at(4, 4));
    REQUIRE(5U == cb.w());
    REQUIRE(5U == cb.h());
    cb.resize(6, 6);
    REQUIRE(6U == cb.w());
    REQUIRE(6U == cb.h());
}

} // end namespace teditor
