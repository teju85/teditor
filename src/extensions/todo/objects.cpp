#include "objects.h"
#include <cstring>
#include "core/utils.h"

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

}  // namespace todo
}  // namespace teditor
