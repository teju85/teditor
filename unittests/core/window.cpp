#include "testutils.h"
#include "core/window.h"
#include "core/buffer.h"
#include "core/editor.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("Windows::default") {
  Windows w;
  REQUIRE(2 == w.size());
  REQUIRE(1 == w.currWinId());
  w.incrementCurrWin();
  REQUIRE(1 == w.currWinId());
  w.decrementCurrWin();
  REQUIRE(1 == w.currWinId());
}

TEST_CASE("Windows::split-and-clear") {
  Windows w;
  REQUIRE(2 == w.size());
  REQUIRE(1 == w.currWinId());
  REQUIRE(w.splitVertically());
  REQUIRE(3 == w.size());
  w.clearAll();
  REQUIRE(2 == w.size());
}

TEST_CASE("Windows::splitVertically") {
  Windows w;
  REQUIRE(2 == w.size());
  REQUIRE(1 == w.currWinId());

  SECTION("oddWidth") {
    w[w.currWinId()]->resize({0, 0}, {31, 10});
    REQUIRE(w.splitVertically());
    REQUIRE(3 == w.size());
    // currently only one vertical split is allowed
    REQUIRE(!w.splitVertically());
    REQUIRE(3 == w.size());
    REQUIRE(Pos2di(0, 0) == w[w.currWinId()]->start());
    REQUIRE(Pos2di(15, 10) == w[w.currWinId()]->dim());
    w.incrementCurrWin();
    REQUIRE(2 == w.currWinId());
    REQUIRE(Pos2di(16, 0) == w[w.currWinId()]->start());
    REQUIRE(Pos2di(15, 10) == w[w.currWinId()]->dim());
  }

  SECTION("evenWidth") {
    w[w.currWinId()]->resize({0, 0}, {30, 10});
    REQUIRE(w.splitVertically());
    REQUIRE(3 == w.size());
    // currently only one vertical split is allowed
    REQUIRE(!w.splitVertically());
    REQUIRE(3 == w.size());
    REQUIRE(Pos2di(0, 0) == w[w.currWinId()]->start());
    REQUIRE(Pos2di(14, 10) == w[w.currWinId()]->dim());
    w.incrementCurrWin();
    REQUIRE(2 == w.currWinId());
    REQUIRE(Pos2di(15, 0) == w[w.currWinId()]->start());
    REQUIRE(Pos2di(15, 10) == w[w.currWinId()]->dim());
  }
}

} // end namespace teditor
