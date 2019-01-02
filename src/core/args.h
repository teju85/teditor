#pragma once

#include <vector>
#include <string>
#include "colors.h"
#include <utility>
#include "utils.h"


namespace teditor {

struct Args {
    Args(int argc, char** argv);
    std::string wrtHomeFolder(const std::string& s) const;
    std::string getHistFile() const { return wrtHomeFolder(histFile); }

    bool quitAfterLoad;
    std::string ttyFile, homeFolder, orgNotesDir;
    std::vector<FileInfo> files;
    int logLevel, cmdMsgBarHeight, cmdMsgBarMultiHeight;
    float pageScrollJump;
    int maxFileHistory, tabSpaces;
    std::string histFile, browserCmd;
};

} // end namespace teditor