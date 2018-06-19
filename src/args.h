#pragma once

#include <vector>
#include <string>
#include "colors.h"
#include <utility>
#include "gtest/gtest_prod.h"


namespace teditor {

typedef std::pair<std::string, int> FileInfo;

struct Args {
    Args(int argc, char** argv);

    bool quitAfterLoad;
    std::string ttyFile;
    std::vector<FileInfo> files;
    int logLevel, cmdMsgBarHeight, cmdMsgBarActiveHeight;
    float pageScrollJump;

private:
    FileInfo readFileInfo(const std::string& arg);

    FRIEND_TEST(Args, ReadFileInfo);
};

} // end namespace teditor
