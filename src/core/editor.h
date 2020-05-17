#pragma once

#include "buffer.h"
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
  void load(const std::string& file, int line);
  void runCmd(const std::string& cmd);
  const std::string clipboard() const;
  void setClipboard(const std::string& in);

  int sendChar(int x, int y, const AttrColor& fg, const AttrColor& bg, char c);
  int sendString(int x, int y, const AttrColor& fg, const AttrColor& bg,
                 const char* str, int len);
  int sendStringf(int x, int y, const AttrColor& fg, const AttrColor& bg,
                  const char* fmt, ...);
  void setColors(AttrColor fg, AttrColor bg);
  void writeCursor(int x, int y);
  void writeChar(uint32_t c, int x, int y);
  void run();
  void requestQuitEventLoop() { quitEventLoop = true; }
  void requestQuitPromptLoop() { quitPromptLoop = true; }
  void requestCancelPromptLoop() { cancelPromptLoop = true; }
  void incrementCurrBuff() { getWindow().incrementCurrBuff(); }
  void decrementCurrBuff() { getWindow().decrementCurrBuff(); }
  void switchToBuff(const std::string& name);
  void killCurrBuff();
  void killOtherBuffs();
  int currBuffId() const { return getWindow().currBuffId(); }
  int buffSize() const { return (int)buffs.size(); }
  void createScratchBuff(bool switchToIt=false);
  void createReadOnlyBuff(const std::string& name, const std::string& contents,
                          bool switchToIt=false);
  void selectCmBar() { cmdMsgBarActive = true; }
  void unselectCmBar() { cmdMsgBarActive = false; }
  Strings fileHistoryToString() const { return fileshist.get(); }
  Strings buffNamesToString() const { return buffs.namesList(); }
  void saveBuffer(Buffer& buf);
  key_t getKey() const;
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
  int pollEvent();
  /** @} */

  void writeLiteral(const char* fmt, ...);
  void clearScreen();
  void resize();
  void clearBackBuff();
  void sendCell(int x, int y, const Cell& c) { backbuff.at(x, y) = c; }
  void writef(const char* fmt, ...);
  void draw();
  void loadFiles();
  void bufResize();
  const AttrColor& getColor(const std::string& name) const;
  int cmBarHeight() const;
  void deleteBuffer(int idx);
  void setCurrBuff(int i) { getWindow().setCurrBuff(i); }
  void checkForModifiedBuffer(Buffer* mlb);
};

}; // end namespace teditor
