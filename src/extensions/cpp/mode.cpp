#include "../base/text.h"
#include "core/buffer.h"
#include "core/pcre.h"

namespace teditor {
namespace cpp {

/** c++ mode */
class CppMode: public text::TextMode {
public:
  CppMode(): text::TextMode("c++") {
    populateKeyMap<CppMode::Keys>(getKeyCmdMap());
    populateColorMap<CppMode::Colors>(getColorMap());
  }

  size_t indent(Buffer& buf, size_t line) {
    if(!(0 < line && line < buf.length())) return 0;
    const auto& prev = buf.at(line-1);
    auto& curr = buf.at(line);
    const auto& prevLine = prev.get();
    const auto& currLine = curr.get();
    Pcre nspace("namespace .*?{");
    size_t prevInd;
    if(nspace.isMatch(prevLine))
      prevInd = 0;
    else if(prevLine[0] == '#')
      prevInd = 0;
    else
      prevInd = prev.indentSize();
    size_t currInd = curr.indentSize();
    if(currLine[0] == '#') return -currInd;
    return prevInd - currInd;
  }

  static Mode* create() { return new CppMode; }

  static bool modeCheck(const std::string& file) {
    static Pcre reg("[.](cpp|c|cxx|cu|h|hpp|hxx|cuh)$");
    return reg.isMatch(file);
  }

private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };
};

REGISTER_MODE(CppMode, "c++");


std::vector<KeyCmdPair> CppMode::Keys::All = {
};

std::vector<NameColorPair> CppMode::Colors::All = {
};

} // end namespace cpp
} // end namespace teditor
