#include <catch.hpp>
#include "extensions/todo/objects.h"
#include <stdexcept>

namespace teditor {
namespace todo {

TEST_CASE("RepeatType") {
  REQUIRE(Repeat_None == strToRepeatType(""));
  REQUIRE(Repeat_None == strToRepeatType("none"));
  REQUIRE(Repeat_Yearly == strToRepeatType("yearly"));
  REQUIRE(Repeat_Monthly == strToRepeatType("monthly"));
  REQUIRE(Repeat_Weekly == strToRepeatType("weekly"));
  REQUIRE(Repeat_Daily == strToRepeatType("daily"));
  REQUIRE_THROWS_AS(strToRepeatType("bad-type"), std::runtime_error);
}

TEST_CASE("CalendarItem") {
  CalendarItem item;
  item.clear();
  REQUIRE(!item.hasStart);
  REQUIRE(!item.hasEnd);
  REQUIRE(item.description.empty());
  REQUIRE(item.repeat == Repeat_None);
}

}  // namespace todo
}  // namespace teditor
