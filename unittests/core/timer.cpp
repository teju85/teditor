#include "core/timer.h"
#include "catch.hpp"

namespace teditor {

TEST_CASE("Timer usage") {
  Timer t;
  REQUIRE_FALSE(t.isRunning());
  REQUIRE_THROWS(t.stop());
  REQUIRE_NOTHROW(t.start());
  REQUIRE(t.isRunning());
  REQUIRE_THROWS(t.start());
  REQUIRE_NOTHROW(t.stop());
  REQUIRE_FALSE(t.isRunning());
}

}  // namespace teditor
