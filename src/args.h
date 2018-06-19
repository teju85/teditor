#pragma once

#include <vector>
#include <string>
#include "colors.h"
#include <utility>
#include "utils.h"


namespace teditor {

struct Args {
    Args(int argc, char** argv);
    std::string wrtHomeFolder(const std::string& s);

    bool quitAfterLoad;
    std::string ttyFile, homeFolder;
    std::vector<FileInfo> files;
    int logLevel, cmdMsgBarHeight, cmdMsgBarActiveHeight;
    float pageScrollJump;
    int maxFileHistory;
    std::string histFile;
};

} // end namespace teditor
