#include "objects.h"
#include <cstring>
#include "core/utils.h"
#include "core/time_utils.h"

namespace teditor {
namespace todo {

RepeatType strToRepeatType(const std::string& str) {
  const auto* cstr = str.c_str();
  if (!strcmp(cstr, "none") || str.empty()) return Repeat_None;
  if (!strcmp(cstr, "yearly")) return Repeat_Yearly;
  if (!strcmp(cstr, "monthly")) return Repeat_Monthly;
  if (!strcmp(cstr, "weekly")) return Repeat_Weekly;
  if (!strcmp(cstr, "daily")) return Repeat_Daily;
  ASSERT(false, "Incorrect RepeatType string passed '%s'!", cstr);
}

void CalendarItem::clear() {
  hasStart = false;
  hasEnd = false;
  repeat = Repeat_None;
  description.clear();
}

TimePoint CalendarItem::getNextOccurence(const TimePoint& pt) const {
  ASSERT(hasStart, "CalendarItem does not have a start point!");
  if (repeat == Repeat_None)    return pt;
  if (repeat == Repeat_Yearly)  return addYear(pt);
  if (repeat == Repeat_Monthly) return addMonth(pt);
  if (repeat == Repeat_Weekly)  return addWeek(pt);
  if (repeat == Repeat_Daily)   return addDay(pt);
  ASSERT(false, "Incorrect RepeatType passed '%d'!", repeat);
}

CalendarItems findMatchesIn(const CalendarItems& items, const TimePoint& start,
                            const TimePoint& end) {
  CalendarItems ret;
  // for (const auto& item : items) {
  // }
  return ret;
}

}  // namespace todo
}  // namespace teditor
