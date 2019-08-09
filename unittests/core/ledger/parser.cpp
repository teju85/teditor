#include "core/ledger/parser.h"
#include "catch.hpp"


namespace teditor {
namespace ledger {

TEST_CASE("Parser::sample") {
  Buffer buf;
  buf.load("samples/ledger/sample.lg");
  Parser p(buf);
  const auto& trans = p.transactions();
  REQUIRE(6 == trans.size());
  const auto& accts = p.accounts();
  REQUIRE(12 == accts.size());
  for(const auto& t : trans) REQUIRE(0 == t.rawBalance());
}

} // end namespace ledger
} // end namespace teditor
