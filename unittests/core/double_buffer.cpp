#include "testutils.h"
#include "core/double_buffer.hpp"
#include "catch.hpp"

namespace teditor {

TEST_CASE("DoubleBuffer") {
  DoubleBuffer<int> db;
  db.current() = 10;
  db.next() = 20;
  REQUIRE(db.current() == 10);
  db.update();
  REQUIRE(db.current() == 20);
  db.update();
  REQUIRE(db.current() == 10);
}

}  // namespace teditor
