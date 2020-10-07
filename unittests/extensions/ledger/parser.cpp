#include "extensions/ledger/parser.h"
#include "catch.hpp"


namespace teditor {
namespace ledger {

TEST_CASE("Parser::sample") {
  Parser p("samples/ledger/sample.lg");
  const auto& trans = p.transactions();
  REQUIRE(7 == trans.size());
  const auto& accts = p.accounts();
  REQUIRE(12 == accts.size());
  for(const auto& t : trans) REQUIRE(0 == t.rawBalance());
}

TEST_CASE("Parser::reload") {
  Parser p("samples/ledger/sample.lg");
  {
    const auto& trans = p.transactions();
    REQUIRE(7 == trans.size());
    const auto& accts = p.accounts();
    REQUIRE(12 == accts.size());
    for(const auto& t : trans) REQUIRE(0 == t.rawBalance());
  }
  p.reload();
  {
    const auto& trans = p.transactions();
    REQUIRE(7 == trans.size());
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
  TimePoint min, max;
  p.minmaxDates(min, max);
  REQUIRE("2018-08-04" == timeToDateStr(min));
  REQUIRE("2018-09-10" == timeToDateStr(max));
}

void parse(const std::string& file) {
  Parser p(file);
}

TEST_CASE("Parser::incorrect - no_alias") {
  REQUIRE_THROWS_AS(parse("samples/ledger/no_alias.lg"), std::runtime_error);
}

} // end namespace ledger
} // end namespace teditor
