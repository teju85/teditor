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

TEST_CASE("Date") {
  SECTION("Correct") {
    Date d("2019/08/09");
    REQUIRE(2019 == d.year);
    REQUIRE(8 == d.month);
    REQUIRE(9 == d.day);
    REQUIRE(d.validate());
  }
  SECTION("Correct - without leading zeros") {
    Date d("2019/8/9");
    REQUIRE(2019 == d.year);
    REQUIRE(8 == d.month);
    REQUIRE(9 == d.day);
    REQUIRE(d.validate());
  }
  SECTION("Incorrect - year") {
    Date d("0/8/9");
    REQUIRE(!d.validate());
  }
  SECTION("Incorrect - month") {
    Date d("2019/0/9");
    REQUIRE(!d.validate());
  }
  SECTION("Incorrect - day1") {
    Date d("2019/1/32");
    REQUIRE(!d.validate());
  }
  SECTION("Incorrect - day2") {
    Date d("2019/2/29");
    REQUIRE(!d.validate());
  }
  SECTION("Incorrect - day3") {
    Date d("2020/2/30");
    REQUIRE(!d.validate());
  }
  SECTION("Incorrect - day4") {
    Date d("2020/4/31");
    REQUIRE(!d.validate());
  }
}

TEST_CASE("Date::<") {
  Date d1("2019/08/09");
  Date d2("2019/08/09");
  Date d3("2019/08/08");
  Date d4("2019/08/10");
  Date d5("2019/07/09");
  Date d6("2019/09/09");
  Date d7("2018/08/09");
  Date d8("2020/08/09");
  REQUIRE(!(d1 < d2));
  REQUIRE(!(d1 < d3));
  REQUIRE(d1 < d4);
  REQUIRE(!(d1 < d5));
  REQUIRE(d1 < d6);
  REQUIRE(!(d1 < d7));
  REQUIRE(d1 < d8);
}

TEST_CASE("Date::==") {
  Date d1("2019/08/09");
  Date d2("2019/08/09");
  Date d3("2019/08/08");
  Date d4("2019/08/10");
  Date d5("2019/07/09");
  Date d6("2019/09/09");
  Date d7("2018/08/09");
  Date d8("2020/08/09");
  REQUIRE(d1 == d2);
  REQUIRE(!(d1 == d3));
  REQUIRE(!(d1 == d4));
  REQUIRE(!(d1 == d5));
  REQUIRE(!(d1 == d6));
  REQUIRE(!(d1 == d7));
  REQUIRE(!(d1 == d8));
}

TEST_CASE("Date::<=") {
  Date d1("2019/08/09");
  Date d2("2019/08/09");
  Date d3("2019/08/08");
  Date d4("2019/08/10");
  Date d5("2019/07/09");
  Date d6("2019/09/09");
  Date d7("2018/08/09");
  Date d8("2020/08/09");
  REQUIRE(d1 <= d2);
  REQUIRE(!(d1 <= d3));
  REQUIRE(d1 <= d4);
  REQUIRE(!(d1 <= d5));
  REQUIRE(d1 <= d6);
  REQUIRE(!(d1 <= d7));
  REQUIRE(d1 <= d8);
}

TEST_CASE("Transaction") {
  Accounts acc;
  REQUIRE(0 == acc.size());
  Transaction t("2019/08/09", "trans");
  REQUIRE("trans" == t.name());
  const auto& d = t.date();
  REQUIRE(2019 == d.year);
  REQUIRE(8 == d.month);
  REQUIRE(9 == d.day);
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
