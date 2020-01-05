#include "core/cmd_msg_bar.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("StringChoices::Test") {
  std::vector<std::string> opts = {
    "list",
    "options",
    "choices"
  };
  StringChoices sc(opts);
  REQUIRE(3 == sc.size());
  REQUIRE("choices" == sc.at(2));
  REQUIRE("options" == sc.getFinalStr(1, "dummy"));
}

TEST_CASE("CmdMsgBar::Default") {
  CmdMsgBar cmBar;
  Point dim = {10, 10};
  REQUIRE(1 == cmBar.length());
  REQUIRE_FALSE(cmBar.usingChoices());
  cmBar.insert("Test: ");
  cmBar.setMinLoc(6);
  REQUIRE(6 == cmBar.at(0).length());
  REQUIRE("Test: " == cmBar.at(0).get());
  REQUIRE(6 == cmBar.getMinStartLoc());
  cmBar.insert('H');
  cmBar.insert('i');
  REQUIRE(1 == cmBar.totalLinesNeeded(dim));
  REQUIRE(8 == cmBar.at(0).length());
  REQUIRE("Test: Hi" == cmBar.at(0).get());
  REQUIRE("Hi" == cmBar.getStr());
  REQUIRE("Hi" == cmBar.getFinalChoice());
  cmBar.setMinLoc(0);
  REQUIRE("Test: Hi" == cmBar.at(0).get());
  cmBar.clear();
  REQUIRE(0 == cmBar.at(0).length());
}

TEST_CASE("CmdMsgBar::UsingStringChoices") {
  std::vector<std::string> opts = {
    "list",
    "options",
    "choices"
  };
  StringChoices sc(opts);
  CmdMsgBar cmBar;
  Point dim = {10, 10};
  REQUIRE(1 == cmBar.length());
  REQUIRE_FALSE(cmBar.usingChoices());
  cmBar.insert("Test: ");
  cmBar.setMinLoc(6);
  REQUIRE(6 == cmBar.at(0).length());
  REQUIRE("Test: " == cmBar.at(0).get());
  REQUIRE(6 == cmBar.getMinStartLoc());
  cmBar.setChoices(&sc);
  REQUIRE(cmBar.usingChoices());
  cmBar.insert('c');
  cmBar.insert('h');
  REQUIRE(2 == cmBar.totalLinesNeeded(dim));
  REQUIRE(8 == cmBar.at(0).length());
  REQUIRE("Test: ch" == cmBar.at(0).get());
  REQUIRE("ch" == cmBar.getStr());
  REQUIRE("choices" == cmBar.getFinalChoice());
  REQUIRE(2 == cmBar.getOptLoc());
  cmBar.setMinLoc(0);
  REQUIRE("Test: ch" == cmBar.at(0).get());
  cmBar.clear();
  REQUIRE(0 == cmBar.at(0).length());
  cmBar.clearChoices();
  REQUIRE_FALSE(cmBar.usingChoices());
}

} // end namespace teditor
