#include "core/ledger/evaluate.h"
#include "catch.hpp"


namespace teditor {
namespace ledger {

TEST_CASE("Evaluate::topAccounts") {
  Buffer buf;
  buf.load("samples/ledger/sample.lg");
  Evaluate e(buf);
  auto top = e.topAccounts();
  REQUIRE(4 == top.size());
  int64_t bal = 0;
  for(const auto& a : top) bal += a.rawBalance();
  REQUIRE(0 == bal);
}

TEST_CASE("Evaluate::minMaxDates") {
  Buffer buf;
  buf.load("samples/ledger/sample.lg");
  Evaluate e(buf);
  Date min("0/0/0"), max("0/0/0");
  e.minmaxDates(min, max);
  REQUIRE(Date("2018/8/4") == min);
  REQUIRE(Date("2018/9/9") == max);
}

} // end namespace ledger
} // end namespace teditor
