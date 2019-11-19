#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for wcstring, strcasestr
#endif

#include <stdint.h>
#include <vector>
#include "editor.h"
#include "keys.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "utils.h"
#include "utf8.h"
#include "command.h"
#include <algorithm>
#include "mode.h"
#include "key_cmd_map.h"
#include "option.h"

namespace teditor {

struct PromptYesNoKeys {
  static std::vector<KeyCmdPair> All;
};

std::vector<KeyCmdPair> PromptYesNoKeys::All = {
  {"n", "prompt-insert-char-quit"},
  {"y", "prompt-insert-char-quit"}
};


Editor::Editor(const std::vector<FileInfo>& _files):
  backbuff(), frontbuff(), lastfg(), lastbg(), cmBar(new CmdMsgBar), buffs(),
  cmBarArr(), windows(), quitEventLoop(false), quitPromptLoop(false),
  cancelPromptLoop(false), cmdMsgBarActive(false), copiedStr(), defcMap(),
  ynMap(), files(_files), fileshist(Option::get("histFile").getStr(),
                                    Option::get("maxHistory").getInt()) {
  DEBUG("Editor: ctor started\n");
  // This array is here only to make sure we get consistent interface to the
  // Window API.
  cmBarArr.push_back(cmBar);
  getCmBarWindow().attachBuffs(&cmBarArr);
  getWindow().attachBuffs(&buffs);
  auto m = Mode::createMode("text");
  defcMap = m->getColorMap();
  populateKeyMap<PromptYesNoKeys>(ynMap, true);
  resize();
  clearBackBuff();
  DEBUG("Editor: ctor finished\n");
}

Editor::~Editor() {
  DEBUG("Editor: dtor started\n");
  fileshist.prune();
  fileshist.store();
  DEBUG("Editor: dtor finished\n");
}

void Editor::saveBuffer(Buffer& buf) {
  if(!buf.isModified()) {
    CMBAR_MSG(*this, "Nothing to save");
    return;
  }
  auto fileName = buf.getFileName();
  if(fileName.empty()) fileName = prompt("File to save: ");
  if(fileName.empty()) {
    CMBAR_MSG(*this, "Empty file name!");
    return;
  }
  if(buf.save(fileName)) CMBAR_MSG(*this, "Wrote %s\n", fileName.c_str());
}

int Editor::cmBarHeight() const {
  if(cmdMsgBarActive && cmBar->usingChoices())
    return Option::get("cmBar::multiheight").getInt();
  return Option::get("cmBar::height").getInt();
}

const AttrColor& Editor::getColor(const std::string& name) const {
  if(buffs.empty()) return defcMap.get(name);
  return getBuff().getColor(name);
}

void Editor::runCmd(const std::string& cmd) {
  if(cmd.empty()) return;
  getCmd(cmd).first(*this);
}

void Editor::createScratchBuff(bool switchToIt) {
  buffs.push_back("*scratch");
  if(switchToIt) setCurrBuff((int)buffs.size() - 1);
}

void Editor::createReadOnlyBuff(const std::string& name,
                                const std::string& contents, bool switchToIt) {
  auto* buf = buffs.push_back(name);
  buf->insert(contents);
  buf->makeReadOnly();
  if(switchToIt) setCurrBuff((int)buffs.size() - 1);
}

///@todo: don't load the same file/dir more than once!
void Editor::loadFiles() {
  DEBUG("loadFiles: started\n");
  if(files.empty()) {
    createScratchBuff(true);
    DEBUG("loadFiles: Added default buffer\n");
    return;
  }
  for(auto& f : files) load(f.first, f.second);
  DEBUG("loadFiles: ended\n");
}

void Editor::load(const std::string& file, int line) {
  auto* buf = new Buffer;
  buf->load(file, line);
  buffs.push_back(buf);
  setCurrBuff((int)buffs.size() - 1);
}

void Editor::switchToBuff(const std::string& name) {
  int idx = 0;
  for(const auto* buff : buffs) {
    if(name == buff->bufferName()) {
      setCurrBuff(idx);
      break;
    }
    ++idx;
  }
}

void Editor::killCurrBuff() {
  deleteBuffer(currBuffId());
  if(buffs.empty()) {
    createScratchBuff(true);
    return;
  }
  int i = currBuffId();
  if(i >= (int)buffs.size()) i = 0;
  setCurrBuff(i);
}

void Editor::killOtherBuffs() {
  for(int i=0;i<(int)buffs.size();++i) {
    if(i != currBuffId()) {
      deleteBuffer(i);
      if(i < currBuffId()) setCurrBuff(currBuffId() - 1);
      --i;
    }
  }
}

void Editor::deleteBuffer(int idx) {
  Buffer* buf = buffs[idx];
  checkForModifiedBuffer(buffs[currBuffId()]);
  auto& f = buf->getFileName();
  if(!f.empty()) {
    int line = buf->getPoint().y;
    fileshist.add(f, line);
  }
  buffs.erase(idx);
}

void Editor::run() {
  loadFiles();
  setCurrBuff(0);
  draw();
  render();
  quitEventLoop = false;
  TrieStatus state = TS_NULL;
  std::string keySoFar, currKey;
  auto& term = Terminal::getInstance();
  while(true) {
    auto& kcMap = getBuff().getKeyCmdMap();
    int status = pollEvent();
    DEBUG("Editor:run: status=%d meta=%u key=%u keystr='%s'\n", status,
          term.mk.getMeta(), term.mk.getKey(), term.mk.toStr().c_str());
    if(status == Terminal::UndefinedSequence) {
      MESSAGE(*this, "Editor:run:: Undefined sequence: %s\n",
              term.getOldSeq().c_str());
      continue;
    } else if(status < 0) break;
    switch(term.type) {
    case Event_Resize:
      clearBackBuff();
      resize();
      break;
    case Event_Key:
      currKey = term.mk.toStr();
      state = kcMap.traverse(currKey);
      if(state == TS_LEAF) {
        CMBAR(*this, "\n");
        std::string cmd = kcMap.getCmd();
        runCmd(cmd);
        kcMap.resetTraversal();
        keySoFar.clear();
      } else if(state == TS_NULL) {
        keySoFar += " " + currKey;
        CMBAR(*this, "Unknown key %s!\n", keySoFar.c_str());
        keySoFar.clear();
        kcMap.resetTraversal();
      } else {
        keySoFar += " " + currKey;
        CMBAR(*this, "%s - ", keySoFar.c_str());
      }
      break;
      ///@todo: mouse support
    default:
      break;
    }
    draw();
    render();
    if(quitEventLoop) break;
  }
}

void Editor::checkForModifiedBuffers() {
  for(auto itr : buffs) checkForModifiedBuffer(itr);
}

void Editor::checkForModifiedBuffer(Buffer* buf) {
  const auto& name = buf->bufferName();
  if(!buf->isModified() || buf->isRO()) return;
  // temp buffers need no saving
  if(name[0] == '*') return;
  if(promptYesNo("Buffer " + name + " modified. Save?")) buf->save();
}

void Editor::writef(const char* fmt, ...) {
  va_list vl;
  va_start(vl, fmt);
  std::string buf = format(fmt, vl);
  va_end(vl);
  Terminal::getInstance().puts(buf);
}

int Editor::sendString(int x, int y, const AttrColor& fg,
                       const AttrColor& bg, const char* str, int len) {
  DEBUG("Editor::sendString: x,y=%d,%d len=%d str='%s'\n", x, y, len, str);
  Cell c = {0, fg, bg};
  int count = 0;
  while(count < len) {
    count += Utf8::char2unicode(&c.ch, str+count);
    sendCell(x, y, c);
    ++x;
  }
  return count;
}

int Editor::sendChar(int x, int y, const AttrColor& fg, const AttrColor& bg,
                     char c) {
  Cell ce = {(Chr)c, fg, bg};
  sendCell(x, y, ce);
  return 1;
}

int Editor::sendStringf(int x, int y, const AttrColor& fg,
                        const AttrColor& bg, const char* fmt, ...) {
  va_list vl;
  va_start(vl, fmt);
  std::string buf = format(fmt, vl);
  va_end(vl);
  return sendString(x, y, fg, bg, buf.c_str(), (int)buf.size());
}

void Editor::resize() {
  auto& term = Terminal::getInstance();
  term.updateTermSize();
  backbuff.resize(term.width(), term.height());
  frontbuff.resize(term.width(), term.height());
  const auto& defaultfg = getColor("defaultfg");
  const auto& defaultbg = getColor("defaultbg");
  frontbuff.clear(defaultfg, defaultbg);
  clearBackBuff();
  bufResize();
  clearScreen();
}

void Editor::clearBackBuff() {
  const auto& defaultfg = getColor("defaultfg");
  const auto& defaultbg = getColor("defaultbg");
  backbuff.clear(defaultfg, defaultbg);
}

Buffer& Editor::getMessagesBuff() {
  bool newOne;
  Buffer& buf = getBuff("*messages", true, newOne);
  if (newOne) {
    buf.makeReadOnly();
  }
  return buf;
}

Buffer& Editor::getBuff(const std::string& name, bool noUndoRedo,
                        bool& newCreated) {
  newCreated = false;
  for(auto itr : buffs) if(itr->bufferName() == name) return *itr;
  Buffer* buf = new Buffer(name, noUndoRedo);
  buffs.push_back(buf);
  newCreated = true;
  return *buf;
}

void Editor::writeLiteral(const char* fmt, ...) {
  va_list vl;
  va_start(vl, fmt);
  std::string buf = format(fmt, vl);
  va_end(vl);
  Terminal::getInstance().puts(buf);
}

void Editor::writeCursor(int x, int y) {
  writeLiteral("\033[%d;%dH", y+1, x+1);
}

void Editor::writeChar(uint32_t c, int x, int y) {
  char buf[8];
  int bw = Utf8::unicode2char(buf, c);
  if(!c) buf[0] = ' '; // replace 0 with whitespace
  writeCursor(x, y);
  Terminal::getInstance().puts(buf, bw);
}

void Editor::setColors(AttrColor fg, AttrColor bg) {
  if((fg == lastfg) && (bg == lastbg)) return;
  auto& term = Terminal::getInstance();
  lastfg = fg;
  lastbg = bg;
  term.puts(Func_Sgr0); // reset attrs
  if(fg.isBold()) term.puts(Func_Bold);
  if(fg.isUnderline()) term.puts(Func_Underline);
  uint8_t fgcol = (uint8_t)fg.color();
  uint8_t bgcol = (uint8_t)bg.color();
  writeLiteral("\033[38;5;%d;48;5;%dm", fgcol, bgcol);
}

void Editor::clearScreen() {
  const auto& defaultfg = getColor("defaultfg");
  const auto& defaultbg = getColor("defaultbg");
  setColors(defaultfg, defaultbg);
  Terminal::getInstance().puts(Func_ClearScreen);
  clearBackBuff();
  render();
}

bool Editor::promptYesNo(const std::string& msg) {
  auto res = prompt(msg+" (y/n)? ", &ynMap);
  return (res == "y");
}

std::string Editor::promptEnum(const std::string& msg, OptionMap& opts) {
  std::string m(msg);
  std::unordered_map<char, std::string> letters;
  KeyCmdMap eMap;
  for(const auto& itr : opts) {
    m += ' ' + itr.first;
    letters[itr.first[0]] = itr.first;
    eMap.add(itr.first[0], "prompt-insert-char-quit");
  }
  m += "? ";
  eMap.add("esc", "prompt-cancel");
  const auto res = prompt(m, &eMap);
  // cancel requested
  if(res.empty()) return "";
  const auto itr = letters.find(res[0]);
  return opts[itr->second];
}

std::string Editor::prompt(const std::string& msg, KeyCmdMap* kcMap,
                           Choices* choices, const std::string& defVal) {
  selectCmBar();
  if(kcMap == nullptr) kcMap = &(cmBar->getKeyCmdMap());
  if(choices != nullptr) cmBar->setChoices(choices);
  cmBar->setMinLoc((int)msg.size());
  bufResize();
  quitPromptLoop = cancelPromptLoop = false;
  cmBar->insert(msg.c_str());
  if(!defVal.empty()) cmBar->insert(defVal.c_str());
  std::string currKey;
  TrieStatus state = TS_NULL;
  draw();
  render();
  auto& term = Terminal::getInstance();
  while(!quitPromptLoop) {
    int status = pollEvent();
    DEBUG("Prompter::loop: status=%d meta=%u key=%u keystr='%s'\n", status,
          term.mk.getMeta(), term.mk.getKey(), term.mk.toStr().c_str());
    if(status == Terminal::UndefinedSequence) {
      MESSAGE(*this, "Prompter::loop: Undefined sequence: %s\n",
              term.getOldSeq().c_str());
      break;
    } else if(status < 0) break;
    if(term.type == Event_Key) {
      currKey = term.mk.toStr();
      state = kcMap->traverse(currKey);
      if(state == TS_LEAF) {
        std::string cmd = kcMap->getCmd();
        runCmd(cmd);
        kcMap->resetTraversal();
      } else if(state == TS_NULL)
        kcMap->resetTraversal();
    }
    draw();
    render();
  }
  auto ret = cmBar->getFinalChoice();
  if(cancelPromptLoop) ret.clear();
  cmBar->clear();
  cmBar->setMinLoc(0);
  if(choices != nullptr) {
    choices->setIdx(cmBar->getOptLoc());
    cmBar->clearChoices();
  }
  unselectCmBar();
  bufResize();
  return ret;
}

void Editor::draw() {
  clearBackBuff();
  windows.draw(*this, cmdMsgBarActive);
}

void Editor::bufResize() {
  int ht = cmBarHeight();
  windows.resize(ht);
}

void Editor::render() {
  auto& term = Terminal::getInstance();
  if(term.bufferResize()) {
    term.disableResize();
    resize();
  }
  int h = (int)frontbuff.h();
  int w = (int)frontbuff.w();
  for(int y=0;y<h;++y) {
    for(int x=0;x<w;) {
      const Cell& back = backbuff.at(x, y);
      Cell& front = frontbuff.at(x, y);
      int wid = back.width();
      if(wid < 1) wid = 1;
      ULTRA_DEBUG("render: y,x=%d,%d back=%u,%hu,%hu front=%u,%hu,%hu\n",
                  y, x, back.ch, back.fg, back.bg, front.ch, front.fg,
                  front.bg);
      if(front == back) {
        x += wid;
        continue;
      }
      front.copy(back);
      setColors(back.fg, back.bg);
      // if character exceeds the screen width
      if(x >= w - wid + 1) {
        ASSERT(false, "Character exceeding screen width is not supported!");
        for (int i=x;i<w;++i) writeChar(' ', i, y);
      } else {
        writeChar(back.ch, x, y);
      }
      x += wid;
    }
  }
  Terminal::getInstance().flush();
}

int Editor::peekEvent(int timeoutMs) {
  struct timeval tv;
  tv.tv_sec = timeoutMs / 1000;
  tv.tv_usec = (timeoutMs - (tv.tv_sec * 1000)) * 1000;
  return Terminal::getInstance().waitAndFill(&tv);
}

} // end namespace teditor
