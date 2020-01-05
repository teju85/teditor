#pragma once

#include "buffer.h"
#include "terminal.h"
#include <stdarg.h>
#include <string>
#include <vector>
#include "logger.h"
#include "buffer.h"
#include "keys.h"
#include <unordered_map>
#include "cell_buffer.h"
#include "cmd_msg_bar.h"
#include "window.h"
#include "file_utils.h"

namespace teditor {

class KeyCmdMap;

typedef std::unordered_map<std::string, std::string> OptionMap;

class Editor {
public:
  Editor(const std::vector<FileInfo>& _files);
  ~Editor();
  void setTitle(const std::string& ti);
  Buffer& getBuff() { return getWindow().getBuff(); }
  const Buffer& getBuff() const { return getWindow().getBuff(); }
  Buffer& getBuff(const std::string& name, bool noUndoRedo, bool& newOne);
  Window& getWindow() { return windows.getWindow(); }
  const Window& getWindow() const { return windows.getWindow(); }
  Window& getCmBarWindow() { return *windows[0]; }
  CmdMsgBar& getCmBar() { return *cmBar; }
  Buffer& getMessagesBuff();
  std::string prompt(const std::string& msg, KeyCmdMap* kcMap=nullptr,
                     Choices* choices=nullptr,const std::string& defVal=std::string());
  bool promptYesNo(const std::string& msg);
  std::string promptEnum(const std::string& msg, OptionMap& opts);
  void load(const std::string& file, size_t line);
  void runCmd(const std::string& cmd);
  const std::string clipboard() const;
  void setClipboard(const std::string& in);

  size_t sendChar(size_t x, size_t y, const AttrColor& fg, const AttrColor& bg,
                  char c);
  size_t sendString(size_t x, size_t y, const AttrColor& fg, const AttrColor& bg,
                    const char* str, size_t len);
  size_t sendStringf(size_t x, size_t y, const AttrColor& fg,
                     const AttrColor& bg, const char* fmt, ...);
  void setColors(AttrColor fg, AttrColor bg);
  void writeCursor(size_t x, size_t y);
  void writeChar(uint32_t c, size_t x, size_t y);
  void run();
  void requestQuitEventLoop() { quitEventLoop = true; }
  void requestQuitPromptLoop() { quitPromptLoop = true; }
  void requestCancelPromptLoop() { cancelPromptLoop = true; }
  void incrementCurrBuff() { getWindow().incrementCurrBuff(); }
  void decrementCurrBuff() { getWindow().decrementCurrBuff(); }
  void switchToBuff(const std::string& name);
  void killCurrBuff();
  void killOtherBuffs();
  size_t currBuffId() const { return getWindow().currBuffId(); }
  size_t buffSize() const { return buffs.size(); }
  void createScratchBuff(bool switchToIt=false);
  void createReadOnlyBuff(const std::string& name, const std::string& contents,
                          bool switchToIt=false);
  void selectCmBar() { cmdMsgBarActive = true; }
  void unselectCmBar() { cmdMsgBarActive = false; }
  Strings fileHistoryToString() const { return fileshist.toString(); }
  Strings buffNamesToString() const { return buffs.namesList(); }
  void saveBuffer(Buffer& buf);
  key_t getKey() const { return Terminal::getInstance().mk.getKey(); }
  bool splitVertically() { return windows.splitVertically(); }
  void clearAllWindows() { return windows.clearAll(); }
  void incrementCurrWin() { windows.incrementCurrWin(); }

private:
  CellBuffer backbuff, frontbuff;
  AttrColor lastfg, lastbg;
  CmdMsgBar* cmBar;
  Buffers buffs, cmBarArr;
  Windows windows;
  bool quitEventLoop, quitPromptLoop, cancelPromptLoop, cmdMsgBarActive;
  ColorMap defcMap;
  KeyCmdMap ynMap;
  std::vector<FileInfo> files;
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
  void sendCell(size_t x, size_t y, const Cell& c) { backbuff.at(x, y) = c; }
  void writef(const char* fmt, ...);
  void draw();
  void loadFiles();
  void bufResize();
  const AttrColor& getColor(const std::string& name) const;
  size_t cmBarHeight() const;
  void deleteBuffer(size_t idx);
  void setCurrBuff(size_t i) { getWindow().setCurrBuff(i); }
  void checkForModifiedBuffer(Buffer* mlb);
};

}; // end namespace teditor
