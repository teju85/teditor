#include "time_utils.h"
#include <iomanip>
#include <sstream>
#include <vector>
#include "utils.h"
#include <cstring>

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

std::string timeToStr(const std::chrono::system_clock::time_point& pt) {
  auto asTime = std::chrono::system_clock::to_time_t(pt);
  char timeStr[256] = {0};
  std::strftime(
    timeStr, sizeof(timeStr), timeFormat().c_str(), std::localtime(&asTime));
  return std::string(timeStr);
}

std::string currentTimeToStr() {
  return timeToStr(std::chrono::system_clock::now());
}

std::chrono::system_clock::time_point timeFromStr(const std::string& dt) {
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
    if (!iss.fail()) {
      auto time_ = std::mktime(&t);
      auto ret = std::chrono::system_clock::from_time_t(time_);
      return ret;
    }
  }
  ASSERT(false, "Incorrect date-time string passed '%s'!", dt.c_str());
}

}  // namespace teditor
