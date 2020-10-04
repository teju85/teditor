#pragma once

#include <string>
#include <chrono>

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
  std::chrono::system_clock::time_point start, end;
  bool hasEnd;
  RepeatType repeat;
  std::string description;
};  // struct CalendarItem

}  // namespace todo
}  // namespace teditor
