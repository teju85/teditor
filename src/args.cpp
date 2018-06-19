#include "args.h"
#include "utils.h"
#include <cstring>
#include "logger.h"
#include <stdlib.h>


namespace teditor {

FileInfo Args::readFileInfo(const std::string& arg) {
    std::string file;
    int line;
    const auto tokens = split(arg, ':');
    if(tokens.size() == 1U) {
        file = tokens[0];
        line = 0;
    } else if(tokens.size() == 2U) {
        file = tokens[0];
        line = str2num(tokens[1]);
    } else {
        ASSERT(false, "File: Bad arg passed. Usage: <file>[:<line>]. '%s'!\n",
               arg.c_str());
    }
    return FileInfo(file, line);
}

std::string Args::wrtHomeFolder(const std::string& s) {
    return homeFolder + '/' + s;
}

Args::Args(int argc, char** argv): quitAfterLoad(false), ttyFile("/dev/tty"),
                                   homeFolder("$HOME/.teditor"), files(),
                                   logLevel(-1), cmdMsgBarHeight(1),
                                   cmdMsgBarActiveHeight(6),
                                   pageScrollJump(0.75f) {
    for(int i=1;i<argc;++i) {
        if(!strcmp(argv[i], "-Q")) {
            quitAfterLoad = true;
        } else if(!strcmp(argv[i], "-v")) {
            ++i;
            ASSERT(i < argc, "'-v' option expects an argument!");
            logLevel = str2num(argv[i]);
        } else if(!strcmp(argv[i], "-tty")) {
            ++i;
            ASSERT(i < argc, "'-tty' option expects an argument!");
            ttyFile = argv[i];
        } else if(!strcmp(argv[i], "-home")) {
            ++i;
            ASSERT(i < argc, "'-home' option expects an argument!");
            homeFolder = argv[i];
        } else if(!strcmp(argv[i], "-cmd-msg-bar-height")) {
            ++i;
            ASSERT(i < argc, "'-cmd-msg-bar-height' expects an argument!");
            cmdMsgBarHeight = str2num(argv[i]);
        } else if(!strcmp(argv[i], "-cmd-msg-bar-active-height")) {
            ++i;
            ASSERT(i < argc, "'-cmd-msg-bar-active-height' expects an argument!");
            cmdMsgBarActiveHeight = str2num(argv[i]);
        } else if(!strcmp(argv[i], "-page-scroll-jump")) {
            ++i;
            ASSERT(i < argc, "'-page-scroll-jump' option expects an argument!");
            pageScrollJump = str2real(argv[i]);
        } else {
            if(argv[i][0] == '-')
                ASSERT(false, "Invalid arg passed! '%s'", argv[i]);
            files.push_back(readFileInfo(argv[i]));
        }
    }
    Logger::setLevel(logLevel);
    homeFolder = expandEnvVars(homeFolder, {"HOME"});
    makeDir(homeFolder);
}

}; // end namespace teditor
