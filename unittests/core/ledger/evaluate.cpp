#include "core/ledger/evaluate.h"
#include "catch.hpp"


namespace teditor {
namespace ledger {

TEST_CASE("Evaluate::sample") {
  Buffer buf;
  buf.load("samples/ledger/sample.lg");
  Evaluate e(buf);
  auto top = e.topAccounts();
  REQUIRE(4 == top.size());
  int64_t bal = 0;
  for(const auto& a : top) bal += a.rawBalance();
  REQUIRE(0 == bal);
}

} // end namespace ledger
} // end namespace teditor
