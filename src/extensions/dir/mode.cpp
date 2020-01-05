#include "../base/readonly.h"
#include "core/buffer.h"
#include "core/file_utils.h"

namespace teditor {
namespace dir {

/** dir mode */
class DirMode: public readonly::ReadOnlyMode {
public:
  DirMode(): readonly::ReadOnlyMode("dir") {
    populateKeyMap<DirMode::Keys>(getKeyCmdMap());
    populateColorMap<DirMode::Colors>(getColorMap());
  }

  void getColorFor(AttrColor& fg, AttrColor& bg, size_t lineNum, size_t pos,
                   const Buffer& b, bool isHighlighted) {
    auto& cm = getColorMap();
    if (isHighlighted) {
      fg = cm.get("highlightfg");
      bg = cm.get("highlightbg");
    } else if (lineNum == 0) {
      fg = cm.get("defaultfg");
      bg = cm.get("defaultbg");
    } else {
      const auto& str = b.at(lineNum).get();
      fg = cm.get(str[2] == 'd' ? "dirfg" : "defaultfg");
      bg = cm.get("defaultbg");
    }
  }

  static Mode* create() { return new DirMode; }

  static bool modeCheck(const std::string& file) { return isDir(file); }

private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };
};

REGISTER_MODE(DirMode, "dir");


std::vector<KeyCmdPair> DirMode::Keys::All = {
  {"c",      "dirmode-copy-file"},
  {"d",      "dirmode-delete-file"},
  {"enter",  "dirmode-open-file"},
  {"r",      "dirmode-rename-file"},
  {"s",      "dirmode-open-special-file"},
};

std::vector<NameColorPair> DirMode::Colors::All = {
  {"dirfg", "Blue"}
};

} // end namespace dir
} // end namespace teditor
