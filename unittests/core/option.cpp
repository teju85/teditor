#include "core/option.h"
#include "core/utils.h"
#include "catch.hpp"

namespace teditor {

TEST_CASE("Option::Test") {
  REQUIRE_THROWS_AS(Option::get("home"), std::runtime_error);
  Option::add("home", "HOME", "teditor home folder", Option::Type::String);
  REQUIRE("HOME" == Option::get("home").getStr());
  REQUIRE_THROWS_AS(Option::get("home").getBool(), std::runtime_error);
  Option::set("home", "HOME1");
  REQUIRE("HOME1" == Option::get("home").getStr());
}

void parseRcFile(const std::string& rc);

TEST_CASE("Option::ExpandOptions") {
  std::vector<FileInfo> files;
  parseArgs(0, nullptr, files);
  REQUIRE(expandEnvVars("$HOME/.teditor/org") ==
          Option::get("orgNotesDir").getStr());
}

TEST_CASE("Option::parseRcFile") {
  parseRcFile("samples/default-rcfile");
  REQUIRE(expandEnvVars("$HOME/.teditor/org") ==
          Option::get("orgNotesDir").getStr());
  REQUIRE_THROWS_AS(parseRcFile("samples/incorrect-rcfile"),
                    std::runtime_error);
}

} // end namespace teditor
