#pragma once

#include <string>
#include "core/time_utils.h"

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
  TimePoint end;
  bool hasEnd;
  RepeatType repeat;
  std::string description;
};  // struct CalendarItem

}  // namespace todo
}  // namespace teditor
