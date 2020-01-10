#pragma once

#include "../base/text.h"
#include <unordered_map>
#include <string>
#include "number.h"

namespace teditor {
namespace calc {

/** calc mode */
class CalcMode: public text::TextMode {
public:
  CalcMode();

  static Mode* create() { return new CalcMode; }
  static bool modeCheck(const std::string& file) { return file == "*calc"; }

private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };

  std::unordered_map<std::string, Num64> vars;
  std::string prompt, lineSeparator;
};  // class CalcMode

}  // namespace calc
}  // namespace teditor
