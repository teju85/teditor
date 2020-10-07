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
  SECTION("Invalid") {
    item.hasStart = false;
    REQUIRE_THROWS_AS(item.getNextOccurence(timeFromStr("2020-01-01")),
                      std::runtime_error);
  }
}

TEST_CASE("findMatchesIn") {
  CalendarItems items{
    CalendarItem{timeFromStr("2020-10-06"), Repeat_None, "task1"},
    CalendarItem{timeFromStr("2020-10-06"), timeFromStr("2020-11-06"),
      Repeat_Daily, "task2"},
    CalendarItem{timeFromStr("2020-10-06"), timeFromStr("2020-11-06"),
      Repeat_Weekly, "task3"},
    CalendarItem{timeFromStr("2020-11-06"), timeFromStr("2020-12-06"),
      Repeat_Weekly, "task4"},
    CalendarItem{timeFromStr("2020-10-07"), timeFromStr("2020-12-07"),
      Repeat_Monthly, "task5"},
    CalendarItem{timeFromStr("2020-11-07"), timeFromStr("2020-12-07"),
      Repeat_Monthly, "task6"},
    CalendarItem{timeFromStr("2020-10-08"), timeFromStr("2021-10-08"),
      Repeat_Yearly, "task7"},
    CalendarItem{timeFromStr("2020-11-09"), timeFromStr("2021-11-09"),
      Repeat_Yearly, "task8"},
    CalendarItem{timeFromStr("2020-10-06"), Repeat_Yearly, "task9"},
  };
  SECTION("bad range") {
    auto start = timeFromStr("2020-10-04"), end = timeFromStr("2020-10-03");
    REQUIRE_THROWS_AS(findMatchesIn(items, start, end), std::runtime_error);
  }
  SECTION("matches") {
    auto start = timeFromStr("2020-10-04"), end = timeFromStr("2020-10-08");
    auto matches = findMatchesIn(items, start, end);
    REQUIRE(8U == matches.size());
    REQUIRE(0 == matches[0].idx);
    REQUIRE("2020-10-06" == timeToDateStr(matches[0].pt));
    REQUIRE(1 == matches[1].idx);
    REQUIRE("2020-10-06" == timeToDateStr(matches[1].pt));
    REQUIRE(2 == matches[2].idx);
    REQUIRE("2020-10-06" == timeToDateStr(matches[2].pt));
    REQUIRE(8 == matches[3].idx);
    REQUIRE("2020-10-06" == timeToDateStr(matches[3].pt));
    REQUIRE(1 == matches[4].idx);
    REQUIRE("2020-10-07" == timeToDateStr(matches[4].pt));
    REQUIRE(4 == matches[5].idx);
    REQUIRE("2020-10-07" == timeToDateStr(matches[5].pt));
    REQUIRE(1 == matches[6].idx);
    REQUIRE("2020-10-08" == timeToDateStr(matches[6].pt));
    REQUIRE(6 == matches[7].idx);
    REQUIRE("2020-10-08" == timeToDateStr(matches[7].pt));
  }
  SECTION("matches due to no end date") {
    auto start = timeFromStr("2021-10-06"), end = timeFromStr("2021-10-06");
    auto matches = findMatchesIn(items, start, end);
    REQUIRE(1U == matches.size());
    REQUIRE(8 == matches[0].idx);
    REQUIRE("2021-10-06" == timeToDateStr(matches[0].pt));
  }
  SECTION("no matches -- future range") {
    auto start = timeFromStr("2021-10-04"), end = timeFromStr("2021-10-04");
    auto matches = findMatchesIn(items, start, end);
    REQUIRE(0U == matches.size());
  }
  SECTION("no matches -- past range") {
    auto start = timeFromStr("2019-10-04"), end = timeFromStr("2019-10-08");
    auto matches = findMatchesIn(items, start, end);
    REQUIRE(0U == matches.size());
  }
}

}  // namespace todo
}  // namespace teditor
