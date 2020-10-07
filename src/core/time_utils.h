#include <ctime>
#include <string>
#include <chrono>

namespace teditor {

typedef std::chrono::system_clock::time_point TimePoint;

bool operator>(const struct timeval& ta, const struct timeval& tb);

struct tm toStructTm(const TimePoint& pt);
TimePoint toTimePoint(struct tm& tm_);
std::string timeToStr(const TimePoint& pt);
std::string timeToDateStr(const TimePoint& pt);
std::string currentTimeToStr();
TimePoint timeFromStr(const std::string& dt);
int dayOfWeek(const TimePoint& pt);
int year(const TimePoint& pt);
int month(const TimePoint& pt);
int day(const TimePoint& pt);

void addSecond(TimePoint& pt);
void addMinute(TimePoint& pt);
void addHour(TimePoint& pt);
void addDay(TimePoint& pt, int times);
void addWeek(TimePoint& pt, int times);
void addMonth(TimePoint& pt);
void addYear(TimePoint& pt);

TimePoint addSecond(const TimePoint& pt);
TimePoint addMinute(const TimePoint& pt);
TimePoint addHour(const TimePoint& pt);
TimePoint addDay(const TimePoint& pt, int times);
TimePoint addWeek(const TimePoint& pt, int times);
TimePoint addMonth(const TimePoint& pt);
TimePoint addYear(const TimePoint& pt);

void weekFor(TimePoint& start, TimePoint& end, const TimePoint& pt);

}  // namespace teditor
