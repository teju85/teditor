#include "testutils.h"
#include "core/window.h"
#include "core/buffer.h"
#include "core/editor.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("Window::Buffer2screen") {
    Buffer ml;
    Window w = setupBuffWin(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
    REQUIRE(4 == ml.length());
    REQUIRE(1 == w.totalLinesNeeded());
    REQUIRE(Pos2di(0, 0) == w.buffer2screen({0, 0}));
    w.resize({0, 0}, {5, 10});
    REQUIRE(2 == w.totalLinesNeeded());
    REQUIRE(Pos2di(4, 2) == w.buffer2screen({4, 1}));
    REQUIRE(Pos2di(0, 3) == w.buffer2screen({5, 1}));
    REQUIRE(Pos2di(4, 5) == w.buffer2screen({9, 2}));
}

TEST_CASE("Window::Screen2buffer") {
    Buffer ml;
    Window w = setupBuffWin(ml, {0, 0}, {30, 10}, "samples/multiline.txt");
    REQUIRE(4 == ml.length());
    REQUIRE(1 == ml.totalLinesNeeded());
    REQUIRE(Pos2di(0, 0) == w.screen2buffer(w.buffer2screen({0, 0})));
    w.resize({0, 0}, {5, 10});
    REQUIRE(2 == w.totalLinesNeeded());
    REQUIRE(Pos2di(4, 1) == w.screen2buffer(w.buffer2screen({4, 1})));
    REQUIRE(Pos2di(5, 1) == w.screen2buffer(w.buffer2screen({5, 1})));
    REQUIRE(Pos2di(9, 2) == w.screen2buffer(w.buffer2screen({9, 2})));
}

} // end namespace teditor
