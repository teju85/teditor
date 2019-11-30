#include "../base/readonly.h"
#include "core/buffer.h"

namespace teditor {
namespace grep {

/** grep mode */
class GrepMode: public readonly::ReadOnlyMode {
public:
  GrepMode(): readonly::ReadOnlyMode("grep") {
    populateKeyMap<GrepMode::Keys>(getKeyCmdMap());
    populateColorMap<GrepMode::Colors>(getColorMap());
  }

  void getColorFor(AttrColor& fg, AttrColor& bg, int lineNum, int pos,
                   const Buffer& b, bool isHighlighted) {
    auto& cmap = getColorMap();
    fg = cmap.get(isHighlighted ? "highlightfg" :
                  lineNum == 0 ? "titlefg" : "defaultfg");
    bg = cmap.get(isHighlighted ? "highlightbg" : "defaultbg");
    const auto& line = b.at(lineNum).get();
    if (lineNum < 4 || pos >= (int)line.size()) return;
    auto loc = line.find_first_of(':');
    if (loc == std::string::npos) return;
    loc = line.find_first_of(':', loc + 1);
    if (loc == std::string::npos) return;
    if (pos < (int)loc) fg = cmap.get("filefg");
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
  {"filefg", "Underline:Yellow"},
  {"titlefg", "Bold:Red"},
};

} // end namespace grep
} // end namespace teditor
