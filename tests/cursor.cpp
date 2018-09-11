#include "cursor.h"
#include "buffer.h"
#include "gtest/gtest.h"


namespace teditor {

TEST(Cursor, Basics) {
    Buffer ml;
    ml.resize({0, 0}, {30, 10});
    Cursor c;
    ASSERT_TRUE(c.isHidden(0));
    ASSERT_TRUE(c.isHidden(0, Cursor::Hidden));
    ASSERT_TRUE(c.isHidden(Cursor::Hidden, 0));
    ASSERT_EQ(1, c.count());
    c.reset(&ml);
    ASSERT_FALSE(c.isHidden(0));
    ASSERT_EQ(1, c.count());
    c.addBack(0, 1);
    ASSERT_FALSE(c.isHidden(1));
    ASSERT_EQ(2, c.count());
    c.remove(0);
    ASSERT_EQ(1, c.count());
    ASSERT_EQ(Pos2d<int>(0, 1), c.at(0));

    c.addBack(1, 2);
    c.addBack(2, 4);
    ASSERT_EQ(3, c.count());
    c.reset(&ml);
    ASSERT_EQ(1, c.count());
    ASSERT_EQ(Pos2d<int>(0, 0), c.at(0));
}

TEST(Cursor, Movements) {
    Buffer ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("samples/sample.cxx", 0);
    auto& cu = ml.getCursor();

    ASSERT_EQ(1, cu.count());
    ASSERT_FALSE(cu.isHidden(0));
    ASSERT_EQ(Pos2d<int>(0, 0), cu.at(0));

    cu.lineEnd(&ml);
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(ml.at(0).length(), 0), cu.at(0));

    cu.home(&ml);
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(0, 0), cu.at(0));

    cu.right(&ml);
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(1, 0), cu.at(0));

    cu.left(&ml);
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(0, 0), cu.at(0));

    cu.down(&ml);
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(0, 1), cu.at(0));

    cu.up(&ml);
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(0, 0), cu.at(0));

    cu.end(&ml);
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(ml.at(ml.length()-1).length(), ml.length()-1), cu.at(0));

    cu.reset(&ml);
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(0, 0), cu.at(0));

    cu.pageDown(&ml, 1.f);
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(0, 9), cu.at(0));

    cu.pageUp(&ml, 1.f);
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(0, 0), cu.at(0));

    cu.nextPara(&ml);
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(0, 3), cu.at(0));

    cu.previousPara(&ml);
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(0, 0), cu.at(0));

    cu.nextWord(&ml);
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(7, 0), cu.at(0));

    cu.previousWord(&ml);
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(0, 0), cu.at(0));
}

TEST(Cursor, Operations) {
    Buffer ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("samples/sample.cxx", 0);
    auto& cu = ml.getCursor();

    cu.remove(0);
    ASSERT_EQ(1, cu.count());

    cu.addBack(0, 1);
    ASSERT_EQ(2, cu.count());
    cu.remove(0);
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(0, 1), cu.at(0));

    cu.addFront(0, 0);
    ASSERT_EQ(2, cu.count());
    cu.remove(0);
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(0, 1), cu.at(0));

    cu.addBack(0, 2);
    ASSERT_EQ(2, cu.count());
    cu.reset(&ml);
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(0, 0), cu.at(0));

    cu.addBack(0, 1);
    cu.addBack(0, 2);
    ASSERT_EQ(3, cu.count());
    cu.clearAllButFirst();
    ASSERT_EQ(1, cu.count());
    ASSERT_EQ(Pos2d<int>(0, 0), cu.at(0));

    cu.reset(&ml);
    ASSERT_TRUE(cu.hasCursor(0));
    ASSERT_FALSE(cu.hasCursor(1));
}


///@todo: add tests for Regions

} // end namespace teditor
