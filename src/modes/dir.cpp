#include "text.h"
#include "core/buffer.h"


namespace teditor {


/** dir mode */
class DirMode: public TextMode {
public:
    DirMode();
    int indent(Buffer& buf, int line) { return 0; }
    const std::string& name() const { return name__; }

    static Mode* create() { return new DirMode; }

    static bool modeCheck(const std::string& file);

private:
    std::string name__;

    struct Keys { static std::vector<KeyCmdPair> All; };
    struct Colors { static std::vector<NameColorPair> All; };
};

REGISTER_MODE(DirMode, "dir");


DirMode::DirMode(): TextMode(), name__("dir") {
    populateKeyMap<DirMode::Keys>(getKeyCmdMap());
    populateColorMap<DirMode::Colors>(getColorMap());
}

bool DirMode::modeCheck(const std::string& file) {
    return isDir(file.c_str());
}


std::vector<KeyCmdPair> DirMode::Keys::All = {
    {"q",      "kill-this-buffer"},
    {"c",      "dirmode-copy-file"},
    {"g",      "dirmode-refresh"},
    {"r",      "dirmode-rename-file"},
    {"d",      "dirmode-delete-file"},
    {"enter",  "dirmode-open-file"}
};

std::vector<NameColorPair> DirMode::Colors::All = {
    {"dirfg", "Blue"}
};


} // end namespace teditor
