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

struct tm toStructTm(const TimePoint& pt) {
  auto asTime = std::chrono::system_clock::to_time_t(pt);
  struct tm* t = std::localtime(&asTime);
  struct tm ret;
  memset(&ret, 0, sizeof(struct tm));
  memcpy(&ret, t, sizeof(struct tm));
  return ret;
}

TimePoint toTimePoint(struct tm& tm_) {
  auto time_ = std::mktime(&tm_);
  return std::chrono::system_clock::from_time_t(time_);
}

std::string timeToStr(const TimePoint& pt) {
  auto tm_ = toStructTm(pt);
  char timeStr[256] = {0};
  std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &tm_);
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
  struct tm t;
  std::memset(&t, 0, sizeof(struct tm));
  int state = 0;
  for (auto c : dt) {
    if (c == '-' && state < 2) {
      ++state;
      continue;
    }
    if (c == ' ' && state == 2) {
      ++state;
      continue;
    }
    if (c == ':' && state >= 3) {
      ++state;
      continue;
    }
    if ('0' > c || c > '9') {
      state = -1;
      break;
    }
    switch (state) {
    case 0:
      t.tm_year = t.tm_year * 10 + c - '0';
      std::cout << dt << " state=" << state << " year=" << t.tm_year << "\n";
      break;
    case 1:
      t.tm_mon = t.tm_mon * 10 + c - '0';
      std::cout << dt << " state=" << state << " mon=" << t.tm_mon << "\n";
      break;
    case 2:
      t.tm_mday = t.tm_mday * 10 + c - '0';
      std::cout << dt << " state=" << state << " mday=" << t.tm_mday << "\n";
      break;
    case 3:
      t.tm_hour = t.tm_hour * 10 + c - '0';
      std::cout << dt << " state=" << state << " hour=" << t.tm_hour << "\n";
      break;
    case 4:
      t.tm_min = t.tm_min * 10 + c - '0';
      std::cout << dt << " state=" << state << " min=" << t.tm_min << "\n";
      break;
    case 5:
      t.tm_sec = t.tm_sec * 10 + c - '0';
      std::cout << dt << " state=" << state << " sec=" << t.tm_sec << "\n";
      break;
    };
  }
  ASSERT(state == 2 || state == 4 || state == 5,
         "Incorrect date-time string passed '%s'!", dt.c_str());
  return toTimePoint(t);
}

int dayOfWeek(const TimePoint& pt) {
  return toStructTm(pt).tm_wday;
}

int year(const TimePoint& pt) {
  return toStructTm(pt).tm_year;
}

int month(const TimePoint& pt) {
  return toStructTm(pt).tm_mon;
}

int day(const TimePoint& pt) {
  return toStructTm(pt).tm_mday;
}

void addSecond(TimePoint& pt) { pt += std::chrono::seconds(1); }

void addMinute(TimePoint& pt) { pt += std::chrono::seconds(60); }

void addHour(TimePoint& pt) { pt += std::chrono::seconds(60 * 60); }

void addDay(TimePoint& pt) { pt += std::chrono::seconds(24 * 60 * 60); }

void addWeek(TimePoint& pt) { pt += std::chrono::seconds(7 * 24 * 60 * 60); }

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

TimePoint addDay(const TimePoint& pt) {
  TimePoint ret = pt;
  addDay(ret);
  return ret;
}

TimePoint addWeek(const TimePoint& pt) {
  TimePoint ret = pt;
  addWeek(ret);
  return ret;
}

}  // namespace teditor
