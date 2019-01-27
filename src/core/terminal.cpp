#include "terminal.h"
#include "utils.h"
#include <cstdio>
#include <sys/stat.h>
#include <stdint.h>


namespace teditor {

const std::string Terminal::EnterMouseSeq = "\x1b[?1000h\x1b[?1002h\x1b[?1015h\x1b[?1006h";
const std::string Terminal::ExitMouseSeq = "\x1b[?1006l\x1b[?1015l\x1b[?1002l\x1b[?1000l";
const int Terminal::Magic = 0432;
const int Terminal::TiFuncs[] = {28, 40, 16, 13, 5, 39, 36, 27, 26, 34, 89, 88};
const int Terminal::TiNFuncs = sizeof(Terminal::TiFuncs) / sizeof(int);
const int Terminal::TiKeys[] = { 66, 68,
                                 /* apparently not a typo; 67 is F10 for whatever reason */
                                 69, 70, 71, 72, 73, 74, 75, 67, 216, 217, 77, 59,
                                 76, 164, 82, 81, 79, 83, 61, 87};
const int Terminal::TiNKeys = sizeof(Terminal::TiKeys) / sizeof(int);

Terminal::Terminal(): keys(), funcs(), termName() {
    termName = env("TERM");
    std::string tidata = loadTerminfo();
    int16_t *header = (int16_t*)&(tidata[0]);
    if((header[1] + header[2]) % 2) {
        // old quirk to align everything on word boundaries
        header[2] += 1;
    }
    const int str_offset = TiNFuncs + header[1] + header[2] + 2 * header[3];
    const int table_offset = str_offset + 2 * header[4];
    for(int i=0;i<TiNKeys;++i) {
        keys.push_back(copyString(tidata, str_offset + 2 * TiKeys[i], table_offset));
    }
    for(int i=0;i<Func_FuncsNum-2;++i) {
        funcs.push_back(copyString(tidata, str_offset + 2 * TiFuncs[i], table_offset));
    }
    funcs.push_back(EnterMouseSeq);
    funcs.push_back(ExitMouseSeq);
}

ColorSupport Terminal::colorSupported() const {
    std::string colorterm = env("COLORTERM");
    if((colorterm.find("truecolor") != std::string::npos) ||
       (colorterm.find("24bit") != std::string::npos)) {
        return ColorSupport_True;
    }
    if((termName.find("-256") != std::string::npos) ||
       termName == "xterm") {
        return ColorSupport_256;
    }
    return ColorSupport_None;
}

std::string Terminal::tryReading(const char* path, const char* term) const {
    char tmp1[2048], tmp2[2048];
    snprintf(tmp1, sizeof(tmp1), "%s/%c/%s", path, term[0], term);
    // for MacOS
    snprintf(tmp2, sizeof(tmp2), "%s/%x/%s", path, term[0], term);
    try {
        return slurp(tmp1);
    } catch(const std::runtime_error& e) {
        return slurp(tmp2);
    }
}

std::string Terminal::loadTerminfo() const {
    // look inside $TERMINFO/
    try {
        std::string termInfo(env("TERMINFO"));
        return tryReading(termInfo.c_str(), termName.c_str());
    } catch(const std::runtime_error& e) {}
    // else, look inside $HOME/.terminfo/
    try {
        std::string home(env("HOME"));
        std::string path = home + "/.terminfo";
        return tryReading(path.c_str(), termName.c_str());
    } catch(const std::runtime_error& e) {}
    // else, look inside dirs mentioned under $TERMINFO_DIRS
    std::string termInfoDirs(env("TERMINFO_DIRS"));
    auto dirs = split(termInfoDirs, ':');
    for(const auto& dir : dirs) {
        if(dir.empty()) continue;
        try {
            return tryReading(dir.c_str(), termName.c_str());
        } catch(const std::runtime_error& e) {}
    }
    // finally... try /usr/share/terminfo
    return tryReading("/usr/share/terminfo", termName.c_str());
}

std::string Terminal::copyString(const std::string& tidata, int str,
                                 int table) const {
    const char* d = &(tidata[0]);
    const int16_t off = *(int16_t*)(d + str);
    const char *src = d + table + off;
    std::string ret(src);
    return ret;
}

} // end namespace teditor
