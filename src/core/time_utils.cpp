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

TimePoint addSecond(const TimePoint& pt) {
  return pt + std::chrono::seconds(1);
}

TimePoint addMinute(const TimePoint& pt) {
  return pt + std::chrono::seconds(60);
}

TimePoint addHour(const TimePoint& pt) {
  return pt + std::chrono::seconds(60 * 60);
}

TimePoint addDay(const TimePoint& pt) {
  return pt + std::chrono::seconds(24 * 60 * 60);
}

TimePoint addWeek(const TimePoint& pt) {
  return pt + std::chrono::seconds(7 * 24 * 60 * 60);
}

}  // namespace teditor
