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
    sleep(2000);
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
          timeToStr(addDay(timeFromStr("2020-01-01 00:23:11"))));
  // month end
  REQUIRE("2020-02-01 00:00:00" ==
          timeToStr(addDay(timeFromStr("2020-01-31"))));
  REQUIRE("2020-05-01 00:00:00" ==
          timeToStr(addDay(timeFromStr("2020-04-30"))));
  // leap year
  REQUIRE("2020-02-29 00:00:00" ==
          timeToStr(addDay(timeFromStr("2020-02-28"))));
  REQUIRE("2019-03-01 00:00:00" ==
          timeToStr(addDay(timeFromStr("2019-02-28"))));
}

TEST_CASE("addWeek") {
  REQUIRE("2020-01-08 00:00:00" ==
          timeToStr(addWeek(timeFromStr("2020-01-01"))));
  // crossing month
  REQUIRE("2020-10-05 00:00:00" ==
          timeToStr(addWeek(timeFromStr("2020-09-28"))));
}

} // end namespace teditor
