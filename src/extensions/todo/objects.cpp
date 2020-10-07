#include "objects.h"
#include <cstring>
#include "core/utils.h"
#include "core/time_utils.h"
#include <iostream>
#include <algorithm>

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

CalendarItem::CalendarItem(
  const TimePoint& s, RepeatType r, const std::string& d):
  start(s), hasStart(true), end(), hasEnd(false), repeat(r), description(d) {
}

CalendarItem::CalendarItem(
  const TimePoint& s, const TimePoint& e, RepeatType r, const std::string& d):
  start(s), hasStart(true), end(e), hasEnd(true), repeat(r), description(d) {
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

bool compareMatches(const MatchItem& a, const MatchItem& b) {
  if (a.pt < b.pt) return true;
  if (a.pt > b.pt) return false;
  return a.idx < b.idx;
}

CalendarMatches findMatchesIn(const CalendarItems& items,
                              const TimePoint& start, const TimePoint& end) {
  ASSERT(start <= end, "findMatchesIn: start time is greater than end!");
  CalendarMatches ret;
  int idx = -1;
  for (const auto& item : items) {
    ++idx;
    if (!item.hasStart) continue;
    auto pt = item.start;
    while (pt <= end) {
      if (item.hasEnd && pt > item.end) break;
      if (pt >= start) {
        MatchItem mi{idx, pt};
        ret.push_back(mi);
      }
      if (item.repeat == Repeat_None) break;
      pt = item.getNextOccurence(pt);
    }
  }
  std::sort(ret.begin(), ret.end(), compareMatches);
  return ret;
}

}  // namespace todo
}  // namespace teditor
