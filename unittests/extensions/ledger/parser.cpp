#include "extensions/ledger/parser.h"
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

TEST_CASE("Parser::topAccounts") {
  Parser p("samples/ledger/sample.lg");
  auto top = p.topAccounts();
  REQUIRE(4 == top.size());
  int64_t bal = 0;
  for(const auto& a : top) bal += a.rawBalance();
  REQUIRE(0 == bal);
}

TEST_CASE("Parser::allAccounts") {
  Parser p("samples/ledger/sample.lg");
  auto all = p.allAccounts();
  REQUIRE(16 == all.size());
  int64_t bal = 0;
  for(const auto& a : all) bal += a.rawBalance();
  REQUIRE(0 == bal);
}

TEST_CASE("Parser::minMaxDates") {
  Parser p("samples/ledger/sample.lg");
  Date min("0/0/0"), max("0/0/0");
  p.minmaxDates(min, max);
  REQUIRE(Date("2018/8/4") == min);
  REQUIRE(Date("2018/9/9") == max);
}

} // end namespace ledger
} // end namespace teditor
