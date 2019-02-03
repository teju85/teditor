#pragma once

#include "buffer.h"
#include "terminal.h"
#include <stdarg.h>
#include <string>
#include <vector>
#include <set>
#include "args.h"
#include "logger.h"
#include "buffer.h"
#include "keys.h"
#include <unordered_map>
#include "files_hist.h"
#include "cell_buffer.h"
#include "cmd_msg_bar.h"


namespace teditor {

class KeyCmdMap;

typedef std::unordered_map<std::string, std::string> OptionMap;

class Editor {
public:
    Editor(const Args& args_, Terminal& t);
    ~Editor();
    int width() const { return term.width(); }
    int height() const { return term.height(); }
    void setTitle(const char* ti) { writef("%c]0;%s%c\n", '\033', ti, '\007'); }
    Buffer& getBuff() { return *buffs[currBuff]; }
    const Buffer& getBuff() const { return *buffs[currBuff]; }
    CmdMsgBar& getCmBar() { return cmBar; }
    Buffer& getMessagesBuff();
    std::string prompt(const std::string& msg, KeyCmdMap* kcMap=nullptr,
                       Choices* choices=nullptr,
                       const std::string& defVal=std::string());
    bool promptYesNo(const std::string& msg);
    std::string promptEnum(const std::string& msg, OptionMap& opts);
    void load(const std::string& file, int line);
    void runCmd(const std::string& cmd);
    void checkForModifiedBuffers();
    void checkForModifiedBuffer(Buffer* mlb);
    bool hasCopy() const { return !copiedStr.empty(); }
    const Strings& copyData() const { return copiedStr; }
    void setCopyData(const Strings& in) { copiedStr = in; }

    int sendChar(int x, int y, const std::string& fg, const std::string& bg,
                 char c);
    int sendString(int x, int y, const std::string& fg, const std::string& bg,
                   const char* str, int len);
    int sendStringf(int x, int y, const std::string& fg, const std::string& bg,
                    const char* fmt, ...);
    void setColors(AttrColor fg, AttrColor bg);
    void writeCursor(int x, int y);
    void writeChar(uint32_t c, int x, int y);
    void run();
    void requestQuitEventLoop() { quitEventLoop = true; }
    void requestQuitPromptLoop() { quitPromptLoop = true; }
    void requestCancelPromptLoop() { cancelPromptLoop = true; }
    void incrementCurrBuff();
    void decrementCurrBuff();
    void switchToBuff(const std::string& name);
    void killCurrBuff();
    void killOtherBuffs();
    int currBuffId() const { return currBuff; }
    int buffSize() const { return (int)buffs.size(); }
    void createScratchBuff(bool switchToIt=false);
    void createReadOnlyBuff(const std::string& name,
                            const std::string& contents, bool switchToIt=false);
    void selectCmBar() { cmdMsgBarActive = true; }
    void unselectCmBar() { cmdMsgBarActive = false; }
    const Args& getArgs() const { return args; }
    Strings fileHistoryToString() const;
    Strings buffNamesToString() const;
    void saveBuffer(Buffer& buf);
    key_t getKey() const { return term.mk.getKey(); }

private:
    CellBuffer backbuff, frontbuff;
    int currBuff;
    Terminal& term;
    AttrColor lastfg, lastbg;
    Args args;
    CmdMsgBar cmBar;
    std::vector<Buffer*> buffs;
    std::set<std::string> buffNames;
    bool quitEventLoop, quitPromptLoop, cancelPromptLoop, cmdMsgBarActive;
    Strings copiedStr;
    ColorMap defcMap;
    KeyCmdMap ynMap;
    FilesHist fileshist;

    void setSignalHandlers();
    void render();
    void updateTermSize();
    void setSignalHandler();
    void setupTios();
    void writeLiteral(const char* fmt, ...);
    void clearScreen();
    void resize();
    void clearBackBuff();
    void sendCell(int x, int y, const Cell& c) { backbuff.at(x, y) = c; }
    void writef(const char* fmt, ...);
    int pollEvent() { return term.waitAndFill(nullptr); }
    int peekEvent(int timeoutMs);
    void draw();
    void loadFiles();
    void bufResize(Buffer* mlb);
    Buffer* getBuff(const std::string& name);
    const AttrColor& getColor(const std::string& name) const;
    int cmBarHeight() const;
    void loadFileHistory();
    void storeFileHistory();
    void pruneFileHistory();
    void addFileHistory(const std::string& file, int line);
    void deleteBuffer(Buffer* buf);
};

}; // end namespace teditor
