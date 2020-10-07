#include "time_utils.h"
#include <iomanip>
#include <sstream>
#include <vector>
#include "utils.h"
#include <cstring>
#include <cstdlib>

namespace teditor {

bool operator>(const struct timeval& ta, const struct timeval& tb) {
  if(ta.tv_sec > tb.tv_sec) return true;
  if((ta.tv_sec == tb.tv_sec) && (ta.tv_usec > tb.tv_usec)) return true;
  return false;
}

const std::string& timeFormat() {
  static std::string fmt("%Y-%m-%d %H:%M:%S");
  return fmt;
}

struct tm toStructTm(const TimePoint& pt) {
  auto asTime = std::chrono::system_clock::to_time_t(pt);
  return *std::localtime(&asTime);
}

TimePoint toTimePoint(struct tm& tm_) {
  auto time_ = std::mktime(&tm_);
  return std::chrono::system_clock::from_time_t(time_);
}

std::string timeToStr(const TimePoint& pt) {
  auto tm_ = toStructTm(pt);
  char timeStr[256] = {0};
  std::strftime(timeStr, sizeof(timeStr), timeFormat().c_str(), &tm_);
  return std::string(timeStr);
}

std::string timeToDateStr(const TimePoint& pt) {
  auto tm_ = toStructTm(pt);
  char timeStr[256] = {0};
  std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d", &tm_);
  return std::string(timeStr);
}

std::string currentTimeToStr() {
  return timeToStr(std::chrono::system_clock::now());
}

TimePoint timeFromStr(const std::string& dt) {
  static std::vector<std::string> fmts = {
    timeFormat(),
    "%Y-%m-%d %H:%M",
    "%Y-%m-%d",
  };
  for (const auto& f : fmts) {
    std::istringstream iss(dt);
    struct tm t;
    std::memset(&t, 0, sizeof(struct tm));
    iss >> std::get_time(&t, f.c_str());
    if (!iss.fail()) return toTimePoint(t);
  }
  ASSERT(false, "Incorrect date-time string passed '%s'!", dt.c_str());
}

int dayOfWeek(const TimePoint& pt) {
  return toStructTm(pt).tm_wday;
}

int year(const TimePoint& pt) {
  return toStructTm(pt).tm_year + 1900;
}

int month(const TimePoint& pt) {
  return toStructTm(pt).tm_mon + 1;
}

int day(const TimePoint& pt) {
  return toStructTm(pt).tm_mday;
}

void addSecond(TimePoint& pt) { pt += std::chrono::seconds(1); }

void addMinute(TimePoint& pt) { pt += std::chrono::seconds(60); }

void addHour(TimePoint& pt) { pt += std::chrono::seconds(60 * 60); }

void addDay(TimePoint& pt, int times) {
  pt += std::chrono::seconds(times * 24 * 60 * 60);
}

void addWeek(TimePoint& pt, int times) {
  pt += std::chrono::seconds(times * 7 * 24 * 60 * 60);
}

void addMonth(TimePoint& pt) {
  auto t = toStructTm(pt);
  ++t.tm_mon;
  if (t.tm_mon >= 12) {
    t.tm_mon = 0;
    ++t.tm_year;
  }
  pt = toTimePoint(t);
}

void addYear(TimePoint& pt) {
  auto t = toStructTm(pt);
  ++t.tm_year;
  pt = toTimePoint(t);
}

TimePoint addSecond(const TimePoint& pt) {
  TimePoint ret = pt;
  addSecond(ret);
  return ret;
}

TimePoint addMinute(const TimePoint& pt) {
  TimePoint ret = pt;
  addMinute(ret);
  return ret;
}

TimePoint addHour(const TimePoint& pt) {
  TimePoint ret = pt;
  addHour(ret);
  return ret;
}

TimePoint addDay(const TimePoint& pt, int times) {
  TimePoint ret = pt;
  addDay(ret, times);
  return ret;
}

TimePoint addWeek(const TimePoint& pt, int times) {
  TimePoint ret = pt;
  addWeek(ret, times);
  return ret;
}

TimePoint addMonth(const TimePoint& pt) {
  TimePoint ret = pt;
  addMonth(ret);
  return ret;
}

TimePoint addYear(const TimePoint& pt) {
  TimePoint ret = pt;
  addYear(ret);
  return ret;
}

void weekFor(TimePoint& start, TimePoint& end, const TimePoint& pt) {
  auto dw = dayOfWeek(pt);
  start = addDay(pt, -dw);
  end = addDay(static_cast<const TimePoint&>(start), 6);
}

}  // namespace teditor
