#include "testutils.h"
#include "core/isearch.h"
#include "core/buffer.h"
#include "catch.hpp"
#include "core/window.h"


namespace teditor {

TEST_CASE("ISearch::Case") {
  Buffers b;
  Window w;
  setupBuffWin(w, b, {0, 0}, {30, 10}, "samples/multiline.txt");
  auto& ml = *b[0];
  REQUIRE(4 == ml.length());
  ISearch is(w, false);
  is.reset();
  REQUIRE(is.emptyAt(0));
  {
    REQUIRE_FALSE(is.updateChoices(""));
  } {
    REQUIRE(is.updateChoices("e"));
    const auto& m0 = is.matchesAt(0);
    REQUIRE(1U == m0.size());
    REQUIRE(3 == m0[0]);
    REQUIRE_FALSE(is.emptyAt(1));
    const auto& m1 = is.matchesAt(1);
    REQUIRE(1U == m1.size());
    REQUIRE_FALSE(is.emptyAt(2));
    const auto& m2 = is.matchesAt(2);
    REQUIRE(2U == m2.size());
    REQUIRE(is.emptyAt(3));
    REQUIRE_THROWS(is.matchesAt(3));
  } {
    REQUIRE(is.updateChoices("el"));
    REQUIRE_FALSE(is.emptyAt(0));
    const auto& m0 = is.matchesAt(0);
    REQUIRE(1U == m0.size());
    REQUIRE(3 == m0[0]);
    REQUIRE(is.emptyAt(1));
    REQUIRE(is.emptyAt(2));
  } {
    REQUIRE(is.updateChoices("e"));
    REQUIRE_FALSE(is.emptyAt(0));
    const auto& m0 = is.matchesAt(0);
    REQUIRE(1U == m0.size());
    REQUIRE(3 == m0[0]);
    REQUIRE_FALSE(is.emptyAt(1));
    const auto& m1 = is.matchesAt(1);
    REQUIRE(1U == m1.size());
    REQUIRE_FALSE(is.emptyAt(2));
    const auto& m2 = is.matchesAt(2);
    REQUIRE(2U == m2.size());
  } {
    REQUIRE(is.updateChoices("es"));
    REQUIRE(is.emptyAt(0));
    REQUIRE_FALSE(is.emptyAt(1));
    REQUIRE(is.emptyAt(2));
  } {
    REQUIRE(is.updateChoices("e"));
    REQUIRE_FALSE(is.updateChoices("e"));
    REQUIRE_FALSE(is.emptyAt(0));
    const auto& m0 = is.matchesAt(0);
    REQUIRE(1U == m0.size());
    REQUIRE(3 == m0[0]);
    REQUIRE_FALSE(is.emptyAt(1));
    const auto& m1 = is.matchesAt(1);
    REQUIRE(1U == m1.size());
    REQUIRE_FALSE(is.emptyAt(2));
    const auto& m2 = is.matchesAt(2);
    REQUIRE(2U == m2.size());
  } {
    REQUIRE(is.updateChoices(""));
    REQUIRE_FALSE(is.updateChoices(""));
  }
}

TEST_CASE("ISearch::ICase") {
  Buffers b;
  Window w;
  setupBuffWin(w, b, {0, 0}, {30, 10}, "samples/multiline.txt");
  auto& ml = *b[0];
  REQUIRE(4 == ml.length());
  ISearch is(w, true);
  is.reset();
  REQUIRE(is.emptyAt(0));
  {
    REQUIRE_FALSE(is.updateChoices(""));
  } {
    REQUIRE(is.updateChoices("E"));
    const auto& m0 = is.matchesAt(0);
    REQUIRE(1U == m0.size());
    REQUIRE(3 == m0[0]);
    REQUIRE_FALSE(is.emptyAt(1));
    const auto& m1 = is.matchesAt(1);
    REQUIRE(1U == m1.size());
    REQUIRE_FALSE(is.emptyAt(2));
    const auto& m2 = is.matchesAt(2);
    REQUIRE(2U == m2.size());
    REQUIRE(is.emptyAt(3));
    REQUIRE_THROWS(is.matchesAt(3));
  } {
    REQUIRE(is.updateChoices("El"));
    REQUIRE_FALSE(is.emptyAt(0));
    const auto& m0 = is.matchesAt(0);
    REQUIRE(1U == m0.size());
    REQUIRE(3 == m0[0]);
    REQUIRE(is.emptyAt(1));
    REQUIRE(is.emptyAt(2));
  } {
    REQUIRE(is.updateChoices("E"));
    REQUIRE_FALSE(is.emptyAt(0));
    const auto& m0 = is.matchesAt(0);
    REQUIRE(1U == m0.size());
    REQUIRE(3 == m0[0]);
    REQUIRE_FALSE(is.emptyAt(1));
    const auto& m1 = is.matchesAt(1);
    REQUIRE(1U == m1.size());
    REQUIRE_FALSE(is.emptyAt(2));
    const auto& m2 = is.matchesAt(2);
    REQUIRE(2U == m2.size());
  } {
    REQUIRE(is.updateChoices("ES"));
    REQUIRE(is.emptyAt(0));
    REQUIRE_FALSE(is.emptyAt(1));
    REQUIRE(is.emptyAt(2));
  } {
    REQUIRE(is.updateChoices("E"));
    REQUIRE_FALSE(is.updateChoices("E"));
    REQUIRE_FALSE(is.emptyAt(0));
    const auto& m0 = is.matchesAt(0);
    REQUIRE(1U == m0.size());
    REQUIRE(3 == m0[0]);
    REQUIRE_FALSE(is.emptyAt(1));
    const auto& m1 = is.matchesAt(1);
    REQUIRE(1U == m1.size());
    REQUIRE_FALSE(is.emptyAt(2));
    const auto& m2 = is.matchesAt(2);
    REQUIRE(2U == m2.size());
  } {
    REQUIRE(is.updateChoices(""));
    REQUIRE_FALSE(is.updateChoices(""));
  }
}

} // end namespace teditor
