#include "../base/readonly.h"
#include "core/buffer.h"

namespace teditor {

/** grep mode */
class GrepMode: public ReadOnlyMode {
public:
  GrepMode(): ReadOnlyMode("grep") {
    populateKeyMap<GrepMode::Keys>(getKeyCmdMap());
    populateColorMap<GrepMode::Colors>(getColorMap());
  }

  static Mode* create() { return new GrepMode; }

  static bool modeCheck(const std::string& file) { return file == "*grep"; }

private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };
};

REGISTER_MODE(GrepMode, "grep");


std::vector<KeyCmdPair> GrepMode::Keys::All = {
  {"enter", "grep-find-file"},
};

std::vector<NameColorPair> GrepMode::Colors::All = {
  {"filefg", "Yellow"},
};

} // end namespace teditor
