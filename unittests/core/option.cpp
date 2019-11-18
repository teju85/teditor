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

} // end namespace teditor
