#include "cell_buffer.h"
#include "gtest/gtest.h"


namespace teditor {

TEST(CellBuffer, Cell) {
    Cell c = {0, 0, 0};
    ASSERT_EQ(0, c.width());
    c.ch = 'c';
    ASSERT_EQ(1, c.width());
    Cell b;
    b.copy(c);
    ASSERT_EQ(c, b);
    b.set('b', 0, 0);
    ASSERT_FALSE(b == c);
}

TEST(CellBuffer, CellBuffer) {
    AttrColor fg=0, bg=1;
    CellBuffer cb(5, 5);
    cb.clear(fg, bg);
    Cell clr = {' ', fg, bg};
    ASSERT_EQ(clr, cb.at(0, 0));
    ASSERT_EQ(clr, cb.at(4, 4));
    ASSERT_EQ(5U, cb.w());
    ASSERT_EQ(5U, cb.h());
    cb.resize(6, 6);
    ASSERT_EQ(6U, cb.w());
    ASSERT_EQ(6U, cb.h());
}

} // end namespace teditor
