#include "core/time_utils.h"
#include <sys/time.h>
#include "catch.hpp"

namespace teditor {

void sleep(int timeMs) {
  clock_t goal = timeMs + clock();
  while (goal > clock());
}

bool compareTestFunc(struct timeval a, struct timeval b) {
  return a > b;
}

TEST_CASE("TimeUtils::Timeval") {
  struct timeval ta, tb;
  gettimeofday(&ta, NULL);
  sleep(500);
  gettimeofday(&tb, NULL);
  REQUIRE(compareTestFunc(tb, ta));
  REQUIRE_FALSE(compareTestFunc(ta, tb));
  REQUIRE_FALSE(compareTestFunc(ta, ta));
}

TEST_CASE("TimeUtils::fromString") {
  SECTION("current-time") {
    auto s = currentTimeToStr();
    auto ts = timeToStr(timeFromStr(s));
    REQUIRE(s == ts);
  }
  SECTION("current-time once more") {
    auto s = currentTimeToStr();
    auto ts = timeToStr(timeFromStr(s));
    REQUIRE(s == ts);
  }
  SECTION("current-time multiple times") {
    auto s = currentTimeToStr();
    auto ts = timeToStr(timeFromStr(s));
    REQUIRE(s == ts);
    sleep(1000);
    s = currentTimeToStr();
    ts = timeToStr(timeFromStr(s));
    REQUIRE(s == ts);
  }
  SECTION("fmt") {
    auto s = timeFromStr("2020-01-01 01:01:01");
    auto ts = timeToStr(s);
    REQUIRE(ts == "2020-01-01 01:01:01");
  }
  SECTION("fmt once more") {
    auto s = timeFromStr("2020-01-01 01:01:01");
    auto ts = timeToStr(s);
    REQUIRE(ts == "2020-01-01 01:01:01");
  }
  SECTION("fmt multiple times") {
    auto s = timeFromStr("2020-01-01 01:01:01");
    auto ts = timeToStr(s);
    REQUIRE(ts == "2020-01-01 01:01:01");
    s = timeFromStr("2020-01-01 01:01:01");
    ts = timeToStr(s);
    REQUIRE(ts == "2020-01-01 01:01:01");
  }
  SECTION("fmt and current-time") {
    auto s = timeFromStr("2020-01-01 01:01:01");
    auto ts = timeToStr(s);
    REQUIRE(ts == "2020-01-01 01:01:01");
    auto ref = currentTimeToStr();
    s = timeFromStr(ref);
    ts = timeToStr(s);
    REQUIRE(ref == ts);
  }
  SECTION("current-time and fmt") {
    auto ref = currentTimeToStr();
    auto s = timeFromStr(ref);
    auto ts = timeToStr(s);
    REQUIRE(ref == ts);
    s = timeFromStr("2020-01-01 01:01:01");
    ts = timeToStr(s);
    REQUIRE(ts == "2020-01-01 01:01:01");
  }
  SECTION("fmt1") {
    auto s = timeFromStr("2020-01-01 01:01");
    auto ts = timeToStr(s);
    REQUIRE(ts == "2020-01-01 01:01:00");
  }
  SECTION("fmt1 once more") {
    auto s = timeFromStr("2020-01-01 01:01");
    auto ts = timeToStr(s);
    REQUIRE(ts == "2020-01-01 01:01:00");
  }
  SECTION("fmt1 multiple times") {
    auto s = timeFromStr("2020-01-01 01:01");
    auto ts = timeToStr(s);
    REQUIRE(ts == "2020-01-01 01:01:00");
    s = timeFromStr("2020-01-01 01:01");
    ts = timeToStr(s);
    REQUIRE(ts == "2020-01-01 01:01:00");
  }
  SECTION("fmt2") {
    auto s = timeFromStr("2020-01-01");
    auto ts = timeToStr(s);
    REQUIRE(ts == "2020-01-01 00:00:00");
  }
  SECTION("fmt2 once more") {
    auto s = timeFromStr("2020-01-01");
    auto ts = timeToStr(s);
    REQUIRE(ts == "2020-01-01 00:00:00");
  }
  SECTION("fmt2 multiple times") {
    auto s = timeFromStr("2020-01-01");
    auto ts = timeToStr(s);
    REQUIRE(ts == "2020-01-01 00:00:00");
    s = timeFromStr("2020-01-01");
    ts = timeToStr(s);
    REQUIRE(ts == "2020-01-01 00:00:00");
  }
  REQUIRE_THROWS_AS(timeFromStr("2020-1-1"), std::runtime_error);
  REQUIRE_THROWS_AS(timeFromStr("2020:01:01"), std::runtime_error);
}

TEST_CASE("dayOfWeek") {
  REQUIRE(3 == dayOfWeek(timeFromStr("2020-09-30")));
  REQUIRE(4 == dayOfWeek(timeFromStr("2020-10-01")));
  REQUIRE(5 == dayOfWeek(timeFromStr("2020-10-02")));
  REQUIRE(6 == dayOfWeek(timeFromStr("2020-10-03")));
  REQUIRE(0 == dayOfWeek(timeFromStr("2020-10-04")));
  REQUIRE(1 == dayOfWeek(timeFromStr("2020-10-05")));
  REQUIRE(2 == dayOfWeek(timeFromStr("2020-10-06")));
  REQUIRE(3 == dayOfWeek(timeFromStr("2020-10-07")));
  REQUIRE(4 == dayOfWeek(timeFromStr("2020-10-08")));
  REQUIRE(5 == dayOfWeek(timeFromStr("2020-10-09")));
  REQUIRE(6 == dayOfWeek(timeFromStr("2020-10-10")));
  REQUIRE(2 == dayOfWeek(timeFromStr("2020-10-06 12:00:10")));
}

TEST_CASE("year") {
  REQUIRE(2020 == year(timeFromStr("2020-09-30")));
  REQUIRE(2020 == year(timeFromStr("2020-10-06 12:00:10")));
}

TEST_CASE("month") {
  REQUIRE(9 == month(timeFromStr("2020-09-30")));
  REQUIRE(10 == month(timeFromStr("2020-10-06 12:00:10")));
}

TEST_CASE("day") {
  REQUIRE(30 == day(timeFromStr("2020-09-30")));
  REQUIRE(1 == day(timeFromStr("2020-10-01")));
  REQUIRE(2 == day(timeFromStr("2020-10-02")));
  REQUIRE(3 == day(timeFromStr("2020-10-03")));
  REQUIRE(4 == day(timeFromStr("2020-10-04")));
  REQUIRE(5 == day(timeFromStr("2020-10-05")));
  REQUIRE(6 == day(timeFromStr("2020-10-06")));
  REQUIRE(7 == day(timeFromStr("2020-10-07")));
  REQUIRE(8 == day(timeFromStr("2020-10-08")));
  REQUIRE(9 == day(timeFromStr("2020-10-09")));
  REQUIRE(10 == day(timeFromStr("2020-10-10")));
  REQUIRE(6 == day(timeFromStr("2020-10-06 12:00:10")));
}

TEST_CASE("addSecond") {
  REQUIRE("2020-01-01 00:00:01" ==
          timeToStr(addSecond(timeFromStr("2020-01-01"))));
  REQUIRE("2020-01-01 00:01:00" ==
          timeToStr(addSecond(timeFromStr("2020-01-01 00:00:59"))));
  // hour end
  REQUIRE("2020-01-01 12:00:00" ==
          timeToStr(addSecond(timeFromStr("2020-01-01 11:59:59"))));
  // day end
  REQUIRE("2020-01-02 00:00:00" ==
          timeToStr(addSecond(timeFromStr("2020-01-01 23:59:59"))));
}

TEST_CASE("addMinute") {
  REQUIRE("2020-01-01 00:01:00" ==
          timeToStr(addMinute(timeFromStr("2020-01-01"))));
  // hour end
  REQUIRE("2020-01-01 01:00:59" ==
          timeToStr(addMinute(timeFromStr("2020-01-01 00:59:59"))));
  // day end
  REQUIRE("2020-01-02 00:00:00" ==
          timeToStr(addMinute(timeFromStr("2020-01-01 23:59:00"))));
}

TEST_CASE("addHour") {
  REQUIRE("2020-01-01 01:23:11" ==
          timeToStr(addHour(timeFromStr("2020-01-01 00:23:11"))));
  // day end
  REQUIRE("2020-01-02 00:00:00" ==
          timeToStr(addHour(timeFromStr("2020-01-01 23:00:00"))));
}

TEST_CASE("addDay") {
  REQUIRE("2020-01-02 00:23:11" ==
          timeToStr(addDay(timeFromStr("2020-01-01 00:23:11"), 1)));
  REQUIRE("2020-01-03 00:23:11" ==
          timeToStr(addDay(timeFromStr("2020-01-01 00:23:11"), 2)));
  REQUIRE("2019-12-31 00:23:11" ==
          timeToStr(addDay(timeFromStr("2020-01-01 00:23:11"), -1)));
  // month end
  REQUIRE("2020-02-01 00:00:00" ==
          timeToStr(addDay(timeFromStr("2020-01-31"), 1)));
  REQUIRE("2020-05-01 00:00:00" ==
          timeToStr(addDay(timeFromStr("2020-04-30"), 1)));
  // leap month
  REQUIRE("2020-02-29 00:00:00" ==
          timeToStr(addDay(timeFromStr("2020-02-28"), 1)));
  REQUIRE("2019-03-01 00:00:00" ==
          timeToStr(addDay(timeFromStr("2019-02-28"), 1)));
  // non-leap month
  REQUIRE("2019-02-28 00:00:00" ==
          timeToStr(addDay(timeFromStr("2019-02-27"), 1)));
  REQUIRE("2019-03-01 00:00:00" ==
          timeToStr(addDay(timeFromStr("2019-02-28"), 1)));
}

TEST_CASE("addWeek") {
  REQUIRE("2020-01-08 00:00:00" ==
          timeToStr(addWeek(timeFromStr("2020-01-01"), 1)));
  // crossing month
  REQUIRE("2020-10-05 00:00:00" ==
          timeToStr(addWeek(timeFromStr("2020-09-28"), 1)));
}

TEST_CASE("addMonth") {
  REQUIRE("2020-02-08 00:00:00" ==
          timeToStr(addMonth(timeFromStr("2020-01-08"))));
  REQUIRE("2020-02-08 00:23:21" ==
          timeToStr(addMonth(timeFromStr("2020-01-08 00:23:21"))));
  // crossing month in a leap year
  REQUIRE("2020-03-01 00:00:00" ==
          timeToStr(addMonth(timeFromStr("2020-01-30"))));
  // crossing month in a non-leap year
  REQUIRE("2019-03-02 00:00:00" ==
          timeToStr(addMonth(timeFromStr("2019-01-30"))));
  // crossing year
  REQUIRE("2020-01-21 00:00:00" ==
          timeToStr(addMonth(timeFromStr("2019-12-21"))));
  // crossing month
  REQUIRE("2020-07-30 00:00:00" ==
          timeToStr(addMonth(timeFromStr("2020-06-30 00:00:00"))));
  REQUIRE("2020-07-01 00:00:00" ==
          timeToStr(addMonth(timeFromStr("2020-05-31 00:00:00"))));
}

TEST_CASE("addYear") {
  REQUIRE("2020-01-08 00:00:00" ==
          timeToStr(addYear(timeFromStr("2019-01-08"))));
  REQUIRE("2021-01-08 00:23:21" ==
          timeToStr(addYear(timeFromStr("2020-01-08 00:23:21"))));
  // leap year
  REQUIRE("2020-02-28 00:00:00" ==
          timeToStr(addYear(timeFromStr("2019-02-28"))));
  REQUIRE("2021-03-01 00:00:00" ==
          timeToStr(addYear(timeFromStr("2020-02-29"))));
}

TEST_CASE("timeToDateStr") {
  REQUIRE("2020-10-05" == timeToDateStr(timeFromStr("2020-10-05 12:00:21")));
  REQUIRE("2020-10-05" == timeToDateStr(timeFromStr("2020-10-05")));
}

TEST_CASE("weekFor") {
  TimePoint start, end;
  weekFor(start, end, timeFromStr("2020-10-07"));
  REQUIRE("2020-10-04" == timeToDateStr(start));
  REQUIRE("2020-10-10" == timeToDateStr(end));
}

} // end namespace teditor
