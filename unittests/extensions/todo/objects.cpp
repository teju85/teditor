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

TEST_CASE("CalendarItem::getNextOccurence") {
  CalendarItem item;
  item.clear();
  item.description = "some task";
  item.hasStart = true;
  item.hasEnd = false;
  SECTION("Repeat_None") {
    item.repeat = Repeat_None;
    item.start = timeFromStr("2020-01-01");
    auto start = item.start;
    start = item.getNextOccurence(start);
    REQUIRE("2020-01-01" == timeToDateStr(start));
    start = item.getNextOccurence(start);
    REQUIRE("2020-01-01" == timeToDateStr(start));
  }
  SECTION("Repeat_Yearly") {
    item.repeat = Repeat_Yearly;
    item.start = timeFromStr("2020-01-01");
    auto start = item.start;
    start = item.getNextOccurence(start);
    REQUIRE("2021-01-01" == timeToDateStr(start));
    start = item.getNextOccurence(start);
    REQUIRE("2022-01-01" == timeToDateStr(start));
  }
  SECTION("Repeat_Monthly") {
    item.repeat = Repeat_Monthly;
    item.start = timeFromStr("2020-01-01");
    auto start = item.start;
    start = item.getNextOccurence(start);
    REQUIRE("2020-02-01" == timeToDateStr(start));
    start = item.getNextOccurence(start);
    REQUIRE("2020-03-01" == timeToDateStr(start));
  }
  SECTION("Repeat_Weekly") {
    item.repeat = Repeat_Weekly;
    item.start = timeFromStr("2020-01-01");
    auto start = item.start;
    start = item.getNextOccurence(start);
    REQUIRE("2020-01-08" == timeToDateStr(start));
    start = item.getNextOccurence(start);
    REQUIRE("2020-01-15" == timeToDateStr(start));
  }
  SECTION("Repeat_Daily") {
    item.repeat = Repeat_Daily;
    item.start = timeFromStr("2020-01-01");
    auto start = item.start;
    start = item.getNextOccurence(start);
    REQUIRE("2020-01-02" == timeToDateStr(start));
    start = item.getNextOccurence(start);
    REQUIRE("2020-01-03" == timeToDateStr(start));
  }
  SECTION("Repeat_Daily with end") {
    item.repeat = Repeat_Daily;
    item.start = timeFromStr("2020-01-01");
    item.hasEnd = true;
    item.end = timeFromStr("2020-01-04");
    auto start = item.start;
    start = item.getNextOccurence(start);
    REQUIRE("2020-01-02" == timeToDateStr(start));
    REQUIRE(start <= item.end);
    start = item.getNextOccurence(start);
    REQUIRE("2020-01-03" == timeToDateStr(start));
    REQUIRE(start <= item.end);
    start = item.getNextOccurence(start);
    REQUIRE("2020-01-04" == timeToDateStr(start));
    REQUIRE(start <= item.end);
    start = item.getNextOccurence(start);
    REQUIRE("2020-01-05" == timeToDateStr(start));
    REQUIRE(start > item.end);
  }
}

}  // namespace todo
}  // namespace teditor
