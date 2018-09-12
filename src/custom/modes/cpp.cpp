#include "text.h"
#include "buffer.h"
#include "pcre.h"


namespace teditor {


/** c++ mode */
class CppMode: public TextMode {
public:
    CppMode();
    int indent(Buffer& buf, int line);
    const std::string& name() const { return name__; }

    static Mode* create() { return new CppMode; }

    static bool modeCheck(const std::string& file);

private:
    std::string name__;

    struct Keys { static std::vector<KeyCmdPair> All; };
    struct Colors { static std::vector<NameColorPair> All; };
};

REGISTER_MODE(CppMode, "c++");


CppMode::CppMode(): TextMode(), name__("c++") {
    populateKeyMap<CppMode::Keys>(getKeyCmdMap());
    populateColorMap<CppMode::Colors>(getColorMap());
}

int CppMode::indent(Buffer& buf, int line) {
    if(!(0 < line && line < buf.length()))
        return 0;
    const auto& prev = buf.at(line-1);
    auto& curr = buf.at(line);
    const auto& prevLine = prev.get();
    const auto& currLine = curr.get();
    Pcre nspace("namespace .*?{");
    int prevInd;
    if(nspace.isMatch(prevLine))
        prevInd = 0;
    else if(prevLine[0] == '#')
        prevInd = 0;
    else
        prevInd = prev.indentSize();
    int currInd = curr.indentSize();
    if(currLine[0] == '#')
        return -currInd;
    return prevInd - currInd;
}

bool CppMode::modeCheck(const std::string& file) {
    static Pcre reg("[.](cpp|c|cxx|cu|h|hpp|cuh)$");
    return reg.isMatch(file);
}


std::vector<KeyCmdPair> CppMode::Keys::All = {
};

std::vector<NameColorPair> CppMode::Colors::All = {
};


} // end namespace teditor
