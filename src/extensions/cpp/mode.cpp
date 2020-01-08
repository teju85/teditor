#include "../base/text.h"
#include "core/buffer.h"
#include "core/pcre.h"
#include "core/parser/nfa.h"

namespace teditor {
namespace cpp {

/** c++ mode */
class CppMode: public text::TextMode {
public:
  CppMode(): text::TextMode("c++"), nspace("namespace.*{") {
    populateKeyMap<CppMode::Keys>(getKeyCmdMap());
    populateColorMap<CppMode::Colors>(getColorMap());
  }

  int indent(Buffer& buf, int line) {
    if(!(0 < line && line < buf.length())) return 0;
    const auto& prev = buf.at(line-1);
    auto& curr = buf.at(line);
    const auto& prevLine = prev.get();
    const auto& currLine = curr.get();
    int prevInd;
    if (nspace.find(prevLine) != parser::NFA::NoMatch)
      prevInd = 0;
    else if (prevLine[0] == '#')
      prevInd = 0;
    else
      prevInd = prev.indentSize();
    int currInd = curr.indentSize();
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

  parser::NFA nspace;
};

REGISTER_MODE(CppMode, "c++");


std::vector<KeyCmdPair> CppMode::Keys::All = {
};

std::vector<NameColorPair> CppMode::Colors::All = {
};

} // end namespace cpp
} // end namespace teditor
