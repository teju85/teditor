#include "core/parser/scanner.h"
#include "catch.hpp"
#include <string>

namespace teditor {
namespace parser {

#define CHECK_SCANNER(ss, i, pt)                 \
  REQUIRE_FALSE(ss.isEof());                     \
  REQUIRE(ss.next(pt) == str[i]);                \
  REQUIRE(pt == Point{i, 0})

TEST_CASE("StringScanner") {
  std::string str("Hello World!");
  StringScanner ss(str);
  Point pt;
  CHECK_SCANNER(ss, 0, pt);
  CHECK_SCANNER(ss, 1, pt);
  CHECK_SCANNER(ss, 2, pt);
  CHECK_SCANNER(ss, 3, pt);
  CHECK_SCANNER(ss, 4, pt);
  CHECK_SCANNER(ss, 5, pt);
  CHECK_SCANNER(ss, 6, pt);
  CHECK_SCANNER(ss, 7, pt);
  CHECK_SCANNER(ss, 8, pt);
  CHECK_SCANNER(ss, 9, pt);
  CHECK_SCANNER(ss, 10, pt);
  CHECK_SCANNER(ss, 11, pt);
  REQUIRE(ss.isEof());
  REQUIRE_THROWS(ss.next(pt));
  ss.setStart({11, 0});
  CHECK_SCANNER(ss, 11, pt);
  REQUIRE(ss.isEof());
  REQUIRE_THROWS(ss.setStart({12, 0}));
  REQUIRE("ello" == ss.at(Point(1, 0), Point(4, 0)));
}

} // end namespace parser
} // end namespace teditor
