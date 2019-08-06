#pragma once

#include "buffer.h"
#include "terminal.h"
#include <stdarg.h>
#include <string>
#include <vector>
#include "args.h"
#include "logger.h"
#include "buffer.h"
#include "keys.h"
#include <unordered_map>
#include "files_hist.h"
#include "cell_buffer.h"
#include "cmd_msg_bar.h"
#include "window.h"


namespace teditor {

class KeyCmdMap;

typedef std::unordered_map<std::string, std::string> OptionMap;

class Editor {
public:
  Editor(const Args& args_);
  ~Editor();
  void setTitle(const char* ti) { writef("%c]0;%s%c\n", '\033', ti, '\007'); }
  Buffer& getBuff() { return *buffs[currBuff]; }
  const Buffer& getBuff() const { return *buffs[currBuff]; }
  Window& getWindow() { return *windows[currWin]; }
  const Window& getWindow() const { return *windows[currWin]; }
  Window& getCmBarWindow() { return *windows[0]; }
  CmdMsgBar& getCmBar() { return cmBar; }
  Buffer& getMessagesBuff();
  std::string prompt(const std::string& msg, KeyCmdMap* kcMap=nullptr,
                     Choices* choices=nullptr,const std::string& defVal=std::string());
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
  void createReadOnlyBuff(const std::string& name, const std::string& contents,
                          bool switchToIt=false);
  void selectCmBar() { cmdMsgBarActive = true; }
  void unselectCmBar() { cmdMsgBarActive = false; }
  const Args& getArgs() const { return args; }
  Strings fileHistoryToString() const { return fileshist.toString(); }
  Strings buffNamesToString() const;
  void saveBuffer(Buffer& buf);
  key_t getKey() const { return Terminal::getInstance().mk.getKey(); }

private:
  CellBuffer backbuff, frontbuff;
  int currBuff, currWin;
  AttrColor lastfg, lastbg;
  Args args;
  CmdMsgBar cmBar;
  Buffers buffs;
  Windows windows;
  bool quitEventLoop, quitPromptLoop, cancelPromptLoop, cmdMsgBarActive;
  Strings copiedStr;
  ColorMap defcMap;
  KeyCmdMap ynMap;
  FilesHist fileshist;

  /**
   * @defgroup InternalTermOps Internal Terminal related operations
   * @{
   */
  void render();
  int pollEvent() { return Terminal::getInstance().waitAndFill(nullptr); }
  int peekEvent(int timeoutMs);
  /** @} */

  void writeLiteral(const char* fmt, ...);
  void clearScreen();
  void resize();
  void clearBackBuff();
  void sendCell(int x, int y, const Cell& c) { backbuff.at(x, y) = c; }
  void writef(const char* fmt, ...);
  void draw();
  void loadFiles();
  void bufResize(Buffer* mlb);
  Buffer* getBuff(const std::string& name);
  const AttrColor& getColor(const std::string& name) const;
  int cmBarHeight() const;
  void deleteBuffer(int idx);
  void setCurrBuff(int i);
};

}; // end namespace teditor
