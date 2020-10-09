#pragma once

#include "../base/text.h"
#include <string>
#include "core/utils.h"
#include "number.h"

namespace teditor {
namespace calc {

typedef std::unordered_map<std::string, Num64> VarMap;

/** calc mode */
class CalcMode: public text::TextMode {
public:
  CalcMode();
  ~CalcMode() { cmds.store(); }

  static Mode* create() { return new CalcMode; }
  static bool modeCheck(const std::string& file) { return file == "*calc"; }

private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };

  std::string prompt, lineSeparator;
  History cmds;
};  // class CalcMode

}  // namespace calc
}  // namespace teditor
