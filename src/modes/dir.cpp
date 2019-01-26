#include "readonly.h"
#include "core/buffer.h"


namespace teditor {


/** dir mode */
class DirMode: public ReadOnlyMode {
public:
    DirMode(): ReadOnlyMode("dir") {
        populateKeyMap<DirMode::Keys>(getKeyCmdMap());
        populateColorMap<DirMode::Colors>(getColorMap());
    }

    static Mode* create() { return new DirMode; }

    static bool modeCheck(const std::string& file) {
        return isDir(file.c_str());
    }

private:
    struct Keys { static std::vector<KeyCmdPair> All; };
    struct Colors { static std::vector<NameColorPair> All; };
};

REGISTER_MODE(DirMode, "dir");


std::vector<KeyCmdPair> DirMode::Keys::All = {
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
