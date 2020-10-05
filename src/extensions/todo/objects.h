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

  void clear();
};  // struct CalendarItem

typedef std::vector<CalendarItem> CalendarItems;

}  // namespace todo
}  // namespace teditor
