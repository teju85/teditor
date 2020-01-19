#include "core/parser/scanner.h"
#include "catch.hpp"
#include <string>
#include "core/file_utils.h"
#include "testutils.h"

namespace teditor {
namespace parser {

#define CHECK_SCANNER(ss, i, pt)  do {           \
    REQUIRE_FALSE(ss.isEof());                   \
    auto c = ss.next(pt);                        \
    REQUIRE(c == str[i]);                        \
    REQUIRE(pt == Point{i, 0});                  \
  } while (0)

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
  REQUIRE("ello" == ss.at(Point(1, 0), Point(4, 0)));
}

TEST_CASE("BufferScanner") {
  Buffer buf;
  setupBuff(buf, {0, 0}, {30, 10}, "samples/hello.txt");
  auto str = slurp("samples/hello.txt");
  BufferScanner bs(buf);
  Point pt;
  CHECK_SCANNER(bs, 0, pt);
  CHECK_SCANNER(bs, 1, pt);
  CHECK_SCANNER(bs, 2, pt);
  CHECK_SCANNER(bs, 3, pt);
  CHECK_SCANNER(bs, 4, pt);
  CHECK_SCANNER(bs, 5, pt);
  CHECK_SCANNER(bs, 6, pt);
  CHECK_SCANNER(bs, 7, pt);
  CHECK_SCANNER(bs, 8, pt);
  CHECK_SCANNER(bs, 9, pt);
  CHECK_SCANNER(bs, 10, pt);
  CHECK_SCANNER(bs, 11, pt);
  CHECK_SCANNER(bs, 12, pt);
  REQUIRE(bs.isEof());
  REQUIRE_THROWS(bs.next(pt));
  REQUIRE("ello" == bs.at(Point(1, 0), Point(4, 0)));
}

#undef CHECK_SCANNER

} // end namespace parser
} // end namespace teditor
