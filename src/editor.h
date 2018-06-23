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

typedef std::unordered_map<std::string, std::string> OptionMap;

class Editor {
public:
    static bool instanceReady() { return inst != nullptr; }
    static Editor& getInstance() { return *inst; }
    int width() const { return tsize.x; }
    int height() const { return tsize.y; }
    void setTitle(const char* title) { sendf("%c]0;%s%c\n", '\033', title, '\007'); }
    Buffer& getBuff() { return *buffs[currBuff]; }
    const Buffer& getBuff() const { return *buffs[currBuff]; }
    CmdMsgBar& getCmBar() { return cmBar; }
    Buffer& getMessagesBuff();
    bool isRegionActive() const { return getBuff().isRegionActive(); }
    std::string prompt(const std::string& msg, KeyCmdMap* kcMap=nullptr,
                       Choices* choices=nullptr,
                       const std::string& defVal=std::string());
    bool promptYesNo(const std::string& msg);
    std::string promptEnum(const std::string& msg, OptionMap& opts);
    void load(const std::string& file, int line);
    void runCmd(const std::string& cmd);
    void startRegion() { getBuff().enableRegions(); }
    void stopRegion() { getBuff().disableRegions(); }
    void checkForModifiedBuffers();
    void checkForModifiedBuffer(Buffer* mlb);
    bool hasCopy() const { return !copiedStr.empty(); }
    const std::vector<std::string>& copyData() const { return copiedStr; }
    void setCopyData(const std::vector<std::string>& in) { copiedStr = in; }

    int getFd() { return inout; }
    void requestResize() { bufferResize = true; }
    void disableResize() { bufferResize = false; }
    int getWinchFd(int id) const { return winchFds[id]; }
    int sendChar(int x, int y, const std::string& fg, const std::string& bg,
                 char c);
    int sendCharNtimes(int x, int y, const std::string& fg,
                       const std::string& bg, char c, int n);
    int sendString(int x, int y, const std::string& fg, const std::string& bg,
                   const char* str, int len);
    int sendStringf(int x, int y, const std::string& fg, const std::string& bg,
                    const char* fmt, ...);
    void setColors(AttrColor fg, AttrColor bg);
    void writeCursor(Pos2d<int>& pos) { writeCursor(pos.x, pos.y); }
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
    std::vector<std::string> fileHistoryToString() const;
    std::vector<std::string> buffNamesToString() const;

private:
    ByteBuffer outbuff;
    CellBuffer backbuff, frontbuff;
    Pos2d<int> tsize;
    int inout;
    Terminal term;
    int winchFds[2];
    struct termios tios, origTios;
    AttrColor lastfg, lastbg;
    bool bufferResize;
    Input input;
    Args args;
    CmdMsgBar cmBar;
    std::vector<Buffer*> buffs;
    std::set<std::string> buffNames;
    int currBuff;
    bool quitEventLoop, quitPromptLoop, cancelPromptLoop, cmdMsgBarActive;
    std::vector<std::string> copiedStr;
    ColorMap defcMap;
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
    void writeCursor(int x, int y);
    void clearScreen();
    void resize();
    void clearBackBuff();
    void sendCell(int x, int y, const Cell& c) { backbuff.at(x, y) = c; }
    void flush() { outbuff.flush(inout); }
    void send(const char* str) { outbuff.puts(str); }
    void sendf(const char* fmt, ...);
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
