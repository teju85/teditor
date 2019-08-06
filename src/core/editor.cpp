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


namespace teditor {

struct PromptYesNoKeys {
  static std::vector<KeyCmdPair> All;
};

std::vector<KeyCmdPair> PromptYesNoKeys::All = {
  {"n", "prompt-insert-char-quit"},
  {"y", "prompt-insert-char-quit"}
};


Editor::Editor(const Args& args_):
  backbuff(), frontbuff(), currBuff(0), lastfg(), lastbg(), args(args_),
  cmBar(), buffs(), windows(), buffNames(), quitEventLoop(false),
  quitPromptLoop(false), cancelPromptLoop(false), cmdMsgBarActive(false),
  copiedStr(), defcMap(), ynMap(),
  fileshist(args.getHistFile(), args.maxFileHistory) {
  // first window is always the cmBar window
  windows.push_back(new Window);
  windows[0]->attachBuff(&cmBar);
  // second window starts as the main window which can then further be split
  windows.push_back(new Window);
  auto m = Mode::createMode("text");
  defcMap = m->getColorMap();
  populateKeyMap<PromptYesNoKeys>(ynMap, true);
  resize();
  clearBackBuff();
}

Editor::~Editor() {
  for(auto itr : buffs) deleteBuffer(itr);
  for(auto itr : windows) delete itr;
  fileshist.prune();
  fileshist.store();
}

Strings Editor::fileHistoryToString() const {
  return fileshist.toString();
}

Strings Editor::buffNamesToString() const {
  Strings ret;
  for(const auto* buff : buffs) ret.push_back(buff->bufferName());
  return ret;
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
  if(cmdMsgBarActive && cmBar.usingChoices()) return args.cmdMsgBarMultiHeight;
  return args.cmdMsgBarHeight;
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
  auto bName = uniquifyName("*scratch", buffNames);
  auto* buf = new Buffer(bName);
  bufResize(buf);
  buffs.push_back(buf);
  buffNames.insert(buf->bufferName());
  if(switchToIt) currBuff = (int)buffs.size() - 1;
}

void Editor::createReadOnlyBuff(const std::string& name,
                                const std::string& contents, bool switchToIt) {
  auto bName = uniquifyName(name, buffNames);
  auto* buf = new Buffer(bName);
  bufResize(buf);
  buffs.push_back(buf);
  buffNames.insert(buf->bufferName());
  buf->insert(contents);
  buf->makeReadOnly();
  if(switchToIt) currBuff = (int)buffs.size() - 1;
}

///@todo: uniquify buffer names!
///@todo: don't load the same file/dir more than once!
void Editor::loadFiles() {
  DEBUG("loadFiles: started\n");
  currBuff = 0;
  if(args.files.empty()) {
    createScratchBuff();
    DEBUG("loadFiles: Added default buffer\n");
    return;
  }
  for(auto& f : args.files) load(f.first, f.second);
  DEBUG("loadFiles: ended\n");
}

void Editor::load(const std::string& file, int line) {
  auto* buf = new Buffer;
  bufResize(buf);
  buf->load(file, line);
  buffs.push_back(buf);
  buffNames.insert(buf->bufferName());
  currBuff = (int)buffs.size() - 1;
}

void Editor::incrementCurrBuff() {
  ++currBuff;
  if(currBuff >= (int)buffs.size())
    currBuff = 0;
}

void Editor::decrementCurrBuff() {
  --currBuff;
  if(currBuff < 0)
    currBuff = (int)buffs.size() - 1;
}

void Editor::switchToBuff(const std::string& name) {
  int idx = 0;
  for(const auto* buff : buffs) {
    if(name == buff->bufferName()) {
      currBuff = idx;
      break;
    }
    ++idx;
  }
}

void Editor::killCurrBuff() {
  checkForModifiedBuffer(buffs[currBuff]);
  buffNames.erase(buffs[currBuff]->bufferName());
  deleteBuffer(buffs[currBuff]);
  buffs.erase(buffs.begin()+currBuff);
  if(buffs.empty()) {
    createScratchBuff();
    currBuff = 0;
    return;
  }
  if(currBuff >= (int)buffs.size()) currBuff = 0;
}

void Editor::killOtherBuffs() {
  auto* buf = buffs[currBuff];
  for(int i=0;i<(int)buffs.size();++i) {
    if(i != currBuff) {
      checkForModifiedBuffer(buffs[i]);
      deleteBuffer(buffs[i]);
    }
  }
  buffs.clear();
  buffNames.clear();
  currBuff = 0;
  buffs.push_back(buf);
  buffNames.insert(buf->bufferName());
}

void Editor::deleteBuffer(Buffer* buf) {
  auto& f = buf->getFileName();
  if(!f.empty()) {
    int line = buf->saveCursors()[0].y;
    fileshist.add(f, line);
  }
  delete buf;
}

void Editor::run() {
  loadFiles();
  currBuff = 0;
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

int Editor::sendString(int x, int y, const std::string& fg,
                       const std::string& bg, const char* str, int len) {
  Cell c = {0, getColor(fg), getColor(bg)};
  int count = 0;
  while(count < len) {
    count += Utf8::char2unicode(&c.ch, str+count);
    sendCell(x, y, c);
    ++x;
  }
  return count;
}

int Editor::sendChar(int x, int y, const std::string& fg, const std::string& bg,
                     char c) {
  Cell ce = {(Chr)c, getColor(fg), getColor(bg)};
  sendCell(x, y, ce);
  return 1;
}

int Editor::sendStringf(int x, int y, const std::string& fg,
                        const std::string& bg, const char* fmt, ...) {
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
  backbuff.clear(defaultfg, defaultbg);
  frontbuff.clear(defaultfg, defaultbg);
  for(auto itr : buffs) bufResize(itr);
  clearScreen();
}

void Editor::clearBackBuff() {
  const auto& defaultfg = getColor("defaultfg");
  const auto& defaultbg = getColor("defaultbg");
  backbuff.clear(defaultfg, defaultbg);
}

Buffer& Editor::getMessagesBuff() {
  Buffer* buf = getBuff("*messages");
  if(buf != nullptr) return *buf;
  auto* buf1 = new Buffer("*messages");
  bufResize(buf1);
  buffs.push_back(buf1);
  buffNames.insert(buf1->bufferName());
  return *buf1;
}

Buffer* Editor::getBuff(const std::string& name) {
  for(auto itr : buffs)
    if(itr->bufferName() == name) return itr;
  return nullptr;
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
  if(kcMap == nullptr) kcMap = &(cmBar.getKeyCmdMap());
  if(choices != nullptr) cmBar.setChoices(choices);
  cmBar.setMinLoc((int)msg.size());
  bufResize(&getBuff());
  quitPromptLoop = cancelPromptLoop = false;
  cmBar.insert(msg.c_str());
  if(!defVal.empty()) cmBar.insert(defVal.c_str());
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
  auto ret = cmBar.getFinalChoice();
  if(cancelPromptLoop) ret.clear();
  cmBar.clear();
  cmBar.setMinLoc(0);
  if(choices != nullptr) {
    choices->setIdx(cmBar.getOptLoc());
    cmBar.clearChoices();
  }
  unselectCmBar();
  bufResize(&getBuff());
  return ret;
}

void Editor::draw() {
  auto& buff = getBuff();
  clearBackBuff();
  DEBUG("draw: drawBuffer\n");
  buff.drawBuffer(*this);
  DEBUG("draw: drawStatusBar\n");
  buff.drawStatusBar(*this);
  DEBUG("draw: cmdMsgBar.drawBuffer\n");
  cmBar.drawBuffer(*this);
  if(cmdMsgBarActive) {
    DEBUG("draw: cmdMsgBar.drawCursor\n");
    cmBar.drawCursor(*this, "cursorbg");
    buff.drawCursor(*this, "inactivecursorbg");
  } else {
    DEBUG("draw: drawCursor\n");
    buff.drawCursor(*this, "cursorbg");
  }
  DEBUG("draw: ended\n");
}

void Editor::bufResize(Buffer* buf) {
  auto& term = Terminal::getInstance();
  int ht = cmBarHeight();
  Pos2di sz(term.width(), term.height());
  sz.y -= ht;
  ///@todo: have the dimensions be solely controlled from Window class
  buf->resize({0, 0}, sz);
  cmBar.resize({0, sz.y}, {sz.x, ht});
  ///@todo: add support for multiple windows
  windows[1]->resize({0, 0}, sz);
  windows[0]->resize({0, sz.y}, {sz.x, ht});
  DEBUG("bufResize: buff-x,y=%d,%d ht=%d\n", sz.x, sz.y, ht);
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
