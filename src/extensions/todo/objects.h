#pragma once

#include <string>
#include "core/time_utils.h"
#include <vector>

namespace teditor {
namespace todo {

enum RepeatType {
  Repeat_None,
  Repeat_Yearly,
  Repeat_Monthly,
  Repeat_Weekly,
  Repeat_Daily,
};  // enum RepeatType

RepeatType strToRepeatType(const std::string& str);

struct CalendarItem {
  TimePoint start;
  bool hasStart;
  TimePoint end;
  bool hasEnd;
  RepeatType repeat;
  std::string description;

  CalendarItem() { clear(); }
  CalendarItem(const TimePoint& s, RepeatType r, const std::string& d);
  CalendarItem(const TimePoint& s, const TimePoint& e, RepeatType r,
               const std::string& d);
  void clear();
  TimePoint getNextOccurence(const TimePoint& pt) const;
};  // struct CalendarItem

struct MatchItem {
  int idx;
  TimePoint pt;
};  // struct MatchItem

typedef std::vector<CalendarItem> CalendarItems;
typedef std::vector<MatchItem> CalendarMatches;

CalendarMatches findMatchesIn(const CalendarItems& items,
                              const TimePoint& start, const TimePoint& end);

}  // namespace todo
}  // namespace teditor
