#include "core/args.h"
#include "core/utils.h"
#include "catch.hpp"

namespace teditor {

TEST_CASE("Args::Test") {
  Args ar(0, nullptr);
  auto home = env("HOME");
  REQUIRE(home + "/.teditor/test" == ar.wrtHomeFolder("test"));
  REQUIRE(home + "/.teditor/ledger.lg" == ar.getLedgerFile());
  REQUIRE(home + "/.teditor/history" == ar.getHistFile());
}

} // end namespace teditor
