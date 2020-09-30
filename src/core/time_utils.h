#include <ctime>
#include <string>
#include <chrono>

namespace teditor {

bool operator>(const struct timeval& ta, const struct timeval& tb);

std::string timeToStr(const std::chrono::system_clock::time_point& pt);
std::string currentTimeToStr();
std::chrono::system_clock::time_point timeFromStr(const std::string& dt);

}  // namespace teditor
