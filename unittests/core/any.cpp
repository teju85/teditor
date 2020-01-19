#include "testutils.h"
#include "core/any.hpp"
#include "catch.hpp"
#include <string>

namespace teditor {

TEST_CASE("Any") {
  Any<bool, int, float, std::string> obj;
  SECTION("default") {
    REQUIRE_FALSE(obj.valid());
    REQUIRE_FALSE(obj.is<bool>());
    REQUIRE_FALSE(obj.is<int>());
    REQUIRE_FALSE(obj.is<float>());
    REQUIRE_FALSE(obj.is<std::string>());
  }
  SECTION("bool") {
    obj.set<bool>(true);
    REQUIRE(obj.valid());
    REQUIRE(obj.is<bool>());
    REQUIRE(obj.get<bool>());
  }
  SECTION("int") {
    obj.set<int>(123);
    REQUIRE(obj.valid());
    REQUIRE(obj.is<int>());
    REQUIRE(obj.get<int>() == 123);
  }
  SECTION("int and float") {
    obj.set<int>(123);
    REQUIRE(obj.valid());
    REQUIRE(obj.is<int>());
    REQUIRE(obj.get<int>() == 123);
    obj.set<float>(123.4f);
    REQUIRE(obj.valid());
    REQUIRE(obj.is<float>());
    REQUIRE(obj.get<float>() == 123.4f);
  }
  SECTION("std::string") {
    obj.set<std::string>("hello world!");
    REQUIRE(obj.valid());
    REQUIRE(obj.is<std::string>());
    REQUIRE(obj.get<std::string>() == "hello world!");
  }
  SECTION("bad usages") {
    REQUIRE_THROWS(obj.get<bool>());
    REQUIRE_THROWS(obj.get<int>());
    REQUIRE_THROWS(obj.get<float>());
    REQUIRE_THROWS(obj.get<std::string>());
  }
  SECTION("get and set") {
    obj.set<int>(123);
    REQUIRE(obj.is<int>());
    REQUIRE(obj.get<int>() == 123);
    obj.get<int>() = 456;
    REQUIRE(obj.get<int>() == 456);
  }
}

}  // namespace teditor
