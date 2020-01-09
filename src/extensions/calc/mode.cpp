#include "../base/readonly.h"
#include "core/buffer.h"

namespace teditor {
namespace calc {

/** calc mode */
class CalcMode: public readonly::ReadOnlyMode {
public:
  CalcMode(): readonly::ReadOnlyMode("calc") {
    populateKeyMap<CalcMode::Keys>(getKeyCmdMap());
    populateColorMap<CalcMode::Colors>(getColorMap());
  }

  static Mode* create() { return new CalcMode; }

  static bool modeCheck(const std::string& file) { return file == "*calc"; }

private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };
};

REGISTER_MODE(CalcMode, "calc");


std::vector<KeyCmdPair> CalcMode::Keys::All = {
};

std::vector<NameColorPair> CalcMode::Colors::All = {
};

} // end namespace calc
} // end namespace teditor
