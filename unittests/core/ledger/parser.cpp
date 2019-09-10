#include "core/ledger/parser.h"
#include "catch.hpp"


namespace teditor {
namespace ledger {

TEST_CASE("Parser::sample") {
  Parser p("samples/ledger/sample.lg");
  const auto& trans = p.transactions();
  REQUIRE(6 == trans.size());
  const auto& accts = p.accounts();
  REQUIRE(12 == accts.size());
  for(const auto& t : trans) REQUIRE(0 == t.rawBalance());
}

TEST_CASE("Parser::reload") {
  Parser p("samples/ledger/sample.lg");
  {
    const auto& trans = p.transactions();
    REQUIRE(6 == trans.size());
    const auto& accts = p.accounts();
    REQUIRE(12 == accts.size());
    for(const auto& t : trans) REQUIRE(0 == t.rawBalance());
  }
  p.reload();
  {
    const auto& trans = p.transactions();
    REQUIRE(6 == trans.size());
    const auto& accts = p.accounts();
    REQUIRE(12 == accts.size());
    for(const auto& t : trans) REQUIRE(0 == t.rawBalance());
  }
}

} // end namespace ledger
} // end namespace teditor
