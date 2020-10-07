#include "extensions/ledger/objects.h"
#include "catch.hpp"


namespace teditor {
namespace ledger {

TEST_CASE("Account::onlyName") {
  Account acc("Acc");
  REQUIRE(0.0 == acc.balance());
  REQUIRE(0 == acc.rawBalance());
  REQUIRE("" == acc.desc());
  REQUIRE("Acc" == acc.name());
  REQUIRE(!acc.isAnAlias("alias"));
  acc.insertAlias("alias");
  REQUIRE(acc.isAnAlias("alias"));
  REQUIRE(!acc.isAnAlias("anotherAlias"));
  acc += 1.0;
  REQUIRE(100 == acc.rawBalance());
  REQUIRE(1.0 == acc.balance());
  acc += -10.0;
  REQUIRE(-900 == acc.rawBalance());
  REQUIRE(-9.0 == acc.balance());
  acc += 0.2;
  REQUIRE(-880 == acc.rawBalance());
  REQUIRE(-8.80== acc.balance());
}

TEST_CASE("Account::name-and-desc") {
  Account acc("Acc", "Description");
  REQUIRE(0 == acc.rawBalance());
  REQUIRE(0.0 == acc.balance());
  REQUIRE("Description" == acc.desc());
  REQUIRE("Acc" == acc.name());
  REQUIRE(!acc.isAnAlias("alias"));
  acc.insertAlias("alias");
  REQUIRE(acc.isAnAlias("alias"));
  REQUIRE(!acc.isAnAlias("anotherAlias"));
}

TEST_CASE("Account::name-and-desc-and-aliases") {
  Aliases as = {"alias", "alias1"};
  Account acc("Acc", "Description", as);
  REQUIRE(0 == acc.rawBalance());
  REQUIRE(0.0 == acc.balance());
  REQUIRE("Description" == acc.desc());
  REQUIRE("Acc" == acc.name());
  REQUIRE(acc.isAnAlias("alias"));
  REQUIRE(acc.isAnAlias("alias1"));
  REQUIRE(!acc.isAnAlias("alias2"));
  acc.insertAlias("alias2");
  REQUIRE(acc.isAnAlias("alias2"));
  REQUIRE(!acc.isAnAlias("anotherAlias"));
}

TEST_CASE("Accounts") {
  Accounts acc;
  REQUIRE(0 == acc.size());
  acc.push_back(Account("a1"));
  REQUIRE(1 == acc.size());
  acc.push_back(Account("a2"));
  auto& a = acc.find("a1");
  REQUIRE("a1" == a.name());
  a += 2.0;
  REQUIRE(200 == acc.find("a1").rawBalance());
  REQUIRE(2.0 == acc.find("a1").balance());
  REQUIRE(2 == acc.size());
  // new account
  auto& b = acc.find("a3");
  REQUIRE(3 == acc.size());
  REQUIRE("a3" == b.name());
  REQUIRE(0 == b.rawBalance());
  REQUIRE(0.0 == b.balance());
}

TEST_CASE("Transaction") {
  Accounts acc;
  REQUIRE(0 == acc.size());
  Transaction t("2019-08-09", "trans");
  REQUIRE("trans" == t.name());
  const auto& d = t.date();
  REQUIRE(2019 == year(d));
  REQUIRE(8 == month(d));
  REQUIRE(9 == day(d));
  SECTION("one credit and debit") {
    t.add("a1", 10.0);
    t.add("a2", -10.0);
    t.updateAccounts(acc);
    REQUIRE(2 == acc.size());
    REQUIRE(1000 == acc.find("a1").rawBalance());
    REQUIRE(10.0 == acc.find("a1").balance());
    REQUIRE(-1000 == acc.find("a2").rawBalance());
    REQUIRE(-10.0 == acc.find("a2").balance());
    REQUIRE(0 == t.rawBalance());
    REQUIRE(0.0 == t.balance());
  }
  SECTION("one credit and default debit") {
    t.add("a1", 10.0);
    t.add("a2");
    t.updateAccounts(acc);
    REQUIRE(2 == acc.size());
    REQUIRE(1000 == acc.find("a1").rawBalance());
    REQUIRE(10.0 == acc.find("a1").balance());
    REQUIRE(-1000 == acc.find("a2").rawBalance());
    REQUIRE(-10.0 == acc.find("a2").balance());
    REQUIRE(0 == t.rawBalance());
    REQUIRE(0.0 == t.balance());
  }
  SECTION("multiple credits and debit") {
    t.add("a1", 10.0);
    t.add("a2", 12.1);
    t.add("a3", -22.1);
    t.updateAccounts(acc);
    REQUIRE(3 == acc.size());
    REQUIRE(1000 == acc.find("a1").rawBalance());
    REQUIRE(10.0 == acc.find("a1").balance());
    REQUIRE(1210 == acc.find("a2").rawBalance());
    REQUIRE(12.1 == acc.find("a2").balance());
    REQUIRE(-2210 == acc.find("a3").rawBalance());
    REQUIRE(-22.1 == acc.find("a3").balance());
    REQUIRE(0 == t.rawBalance());
    REQUIRE(0.0 == t.balance());
  }
  SECTION("multiple credits and default debit") {
    t.add("a1", 10.0);
    t.add("a2", 12.1);
    t.add("a3");
    t.updateAccounts(acc);
    REQUIRE(3 == acc.size());
    REQUIRE(1000 == acc.find("a1").rawBalance());
    REQUIRE(10.0 == acc.find("a1").balance());
    REQUIRE(1210 == acc.find("a2").rawBalance());
    REQUIRE(12.1 == acc.find("a2").balance());
    REQUIRE(-2210 == acc.find("a3").rawBalance());
    REQUIRE(-22.1 == acc.find("a3").balance());
    REQUIRE(0 == t.rawBalance());
    REQUIRE(0.0 == t.balance());
  }
  SECTION("multiple credits and multiple debits") {
    t.add("a1", 10.0);
    t.add("a2", 12.1);
    t.add("a3", -12.0);
    t.add("a4", -10.1);
    t.updateAccounts(acc);
    REQUIRE(4 == acc.size());
    REQUIRE(1000 == acc.find("a1").rawBalance());
    REQUIRE(10.0 == acc.find("a1").balance());
    REQUIRE(1210 == acc.find("a2").rawBalance());
    REQUIRE(12.1 == acc.find("a2").balance());
    REQUIRE(-1200 == acc.find("a3").rawBalance());
    REQUIRE(-12.0 == acc.find("a3").balance());
    REQUIRE(-1010 == acc.find("a4").rawBalance());
    REQUIRE(-10.1 == acc.find("a4").balance());
    REQUIRE(0 == t.rawBalance());
    REQUIRE(0.0 == t.balance());
  }
  SECTION("incorrect") {
    t.add("a1", 10.0);
    t.add("a2", 12.1);
    t.add("a3", -10.0);
    t.add("a4", -10.1);
    t.updateAccounts(acc);
    REQUIRE(4 == acc.size());
    REQUIRE(1000 == acc.find("a1").rawBalance());
    REQUIRE(10.0 == acc.find("a1").balance());
    REQUIRE(1210 == acc.find("a2").rawBalance());
    REQUIRE(12.1 == acc.find("a2").balance());
    REQUIRE(-1000 == acc.find("a3").rawBalance());
    REQUIRE(-10.0 == acc.find("a3").balance());
    REQUIRE(-1010 == acc.find("a4").rawBalance());
    REQUIRE(-10.1 == acc.find("a4").balance());
    REQUIRE(200 == t.rawBalance());
    REQUIRE(2.0 == t.balance());
  }
}

} // end namespace ledger
} // end namespace teditor
