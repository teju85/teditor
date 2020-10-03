#include <ctime>
#include <string>
#include <chrono>

namespace teditor {

bool operator>(const struct timeval& ta, const struct timeval& tb);

struct tm toStructTm(const std::chrono::system_clock::time_point& pt);
std::chrono::system_clock::time_point toTimePoint(struct tm& tm_);
std::string timeToStr(const std::chrono::system_clock::time_point& pt);
std::string currentTimeToStr();
std::chrono::system_clock::time_point timeFromStr(const std::string& dt);
int dayOfWeek(const std::chrono::system_clock::time_point& pt);

}  // namespace teditor
