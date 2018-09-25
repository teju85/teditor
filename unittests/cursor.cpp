#include "testutils.h"
#include "cursor.h"
#include "buffer.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("Cursor::Basics") {
    Buffer ml;
    ml.resize({0, 0}, {30, 10});
    Cursor c;
    REQUIRE(1 == c.count());
    c.reset(&ml);
    REQUIRE(1 == c.count());
    c.addBack(0, 1);
    REQUIRE(2 == c.count());
    c.remove(0);
    REQUIRE(1 == c.count());
    REQUIRE(Pos2d<int>(0, 1) == c.at(0));

    c.addBack(1, 2);
    c.addBack(2, 4);
    REQUIRE(3 == c.count());
    c.reset(&ml);
    REQUIRE(1 == c.count());
    REQUIRE(Pos2d<int>(0, 0) == c.at(0));
}

TEST_CASE("Cursor::Movements") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    auto& cu = ml.getCursor();

    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));

    cu.lineEnd(&ml);
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(ml.at(0).length(), 0) == cu.at(0));

    cu.home(&ml);
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));

    cu.right(&ml);
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(1, 0) == cu.at(0));

    cu.left(&ml);
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));

    cu.down(&ml);
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(0, 1) == cu.at(0));

    cu.up(&ml);
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));

    cu.end(&ml);
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(ml.at(ml.length()-1).length(), ml.length()-1) == cu.at(0));

    cu.reset(&ml);
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));

    cu.pageDown(&ml, 1.f);
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(0, 9) == cu.at(0));

    cu.pageUp(&ml, 1.f);
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));

    cu.nextPara(&ml);
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(0, 3) == cu.at(0));

    cu.previousPara(&ml);
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));

    cu.nextWord(&ml);
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(7, 0) == cu.at(0));

    cu.previousWord(&ml);
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));
}

TEST_CASE("Cursor::Operations") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "samples/sample.cxx");
    auto& cu = ml.getCursor();

    cu.remove(0);
    REQUIRE(1 == cu.count());

    cu.addBack(0, 1);
    REQUIRE(2 == cu.count());
    cu.remove(0);
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(0, 1) == cu.at(0));

    cu.addFront(0, 0);
    REQUIRE(2 == cu.count());
    cu.remove(0);
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(0, 1) == cu.at(0));

    cu.addBack(0, 2);
    REQUIRE(2 == cu.count());
    cu.reset(&ml);
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));

    cu.addBack(0, 1);
    cu.addBack(0, 2);
    REQUIRE(3 == cu.count());
    cu.clearAllButFirst();
    REQUIRE(1 == cu.count());
    REQUIRE(Pos2d<int>(0, 0) == cu.at(0));

    cu.reset(&ml);
    REQUIRE(cu.hasCursor(0));
    REQUIRE_FALSE(cu.hasCursor(1));
}


///@todo: add tests for Regions

} // end namespace teditor
