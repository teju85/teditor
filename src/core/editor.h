#pragma once

#include "buffer.h"
#include "terminal.h"
#include <termios.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <set>
#include "args.h"
#include "logger.h"
#include "buffer.h"
#include "keys.h"
#include "input.h"
#include <unordered_map>
#include "files_hist.h"
#include "byte_buffer.h"
#include "cell_buffer.h"
#include "cmd_msg_bar.h"


namespace teditor {

class KeyCmdMap;

typedef std::unordered_map<std::string, std::string> OptionMap;

class Editor {
public:
    static bool instanceReady() { return inst != nullptr; }
    static Editor& getInstance() { return *inst; }
    int width() const { return tsize.x; }
    int height() const { return tsize.y; }
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

    int getFd() { return inout; }
    void requestResize() { bufferResize = true; }
    void disableResize() { bufferResize = false; }
    int getWinchFd(int id) const { return winchFds[id]; }
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
    Input& getInput() { return input; }
    void incrementCurrBuff();
    void decrementCurrBuff();
    void switchToBuff(const std::string& name);
    void killCurrBuff();
    void killOtherBuffs();
    int currBuffId() const { return currBuff; }
    int buffSize() const { return (int)buffs.size(); }
    void createScratchBuff(bool switchToIt=false);
    void selectCmBar() { cmdMsgBarActive = true; }
    void unselectCmBar() { cmdMsgBarActive = false; }
    const Args& getArgs() const { return args; }
    void addFileHistory(const std::string& file, int line);
    Strings fileHistoryToString() const;
    Strings buffNamesToString() const;

private:
    ByteBuffer outbuff;
    CellBuffer backbuff, frontbuff;
    Pos2d<int> tsize;
    int inout, currBuff, winchFds[2];
    Terminal term;
    struct termios tios, origTios;
    AttrColor lastfg, lastbg;
    bool bufferResize;
    Input input;
    Args args;
    CmdMsgBar cmBar;
    std::vector<Buffer*> buffs;
    std::set<std::string> buffNames;
    bool quitEventLoop, quitPromptLoop, cancelPromptLoop, cmdMsgBarActive;
    Strings copiedStr;
    ColorMap defcMap;
    KeyCmdMap ynMap;
    FilesHist fileshist;

    static const int OutBuffSize;

    static Editor* inst;

    Editor(const Args& args_);
    ~Editor();
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
    void flush() { outbuff.flush(inout); }
    void writef(const char* fmt, ...);
    void hideCursor() { outbuff.puts(term.func(Func_HideCursor)); }
    void showCursor() { outbuff.puts(term.func(Func_ShowCursor)); }
    void enableMouse() { outbuff.puts(term.func(Func_EnterMouse)); }
    void disableMouse() { outbuff.puts(term.func(Func_ExitMouse)); }
    int pollEvent();
    int peekEvent(int timeoutMs);
    void draw();
    void loadFiles();
    void mlResize(Buffer* mlb);
    Buffer* getBuff(const std::string& name);
    const AttrColor& getColor(const std::string& name) const;
    int cmBarHeight() const;
    void loadFileHistory();
    void storeFileHistory();
    void pruneFileHistory();

    friend class SingletonHandler<Editor, Args>;
};

}; // end namespace teditor
