#include "cmd_msg_bar.h"
#include <string.h>
#include "mode.h"
#include "window.h"
#include "logger.h"
#include "editor.h"

namespace teditor {

bool Choices::match(const std::string& line, const std::string& str) const {
  return filter(line, str);
}

bool Choices::match(int idx, const std::string& str) const {
  return match(at(idx), str);
}


StringChoices::StringChoices(const Strings& arr, ChoicesFilter cf):
  Choices(cf), options(arr) {
}

std::string StringChoices::getFinalStr(int idx, const std::string& str) const {
  return at(idx);
}


CmdMsgBar::CmdMsgBar(): Buffer(), minLoc(0), choices(), optLoc(0) {
  mode = Mode::createMode("cmbar");
}

void CmdMsgBar::draw(Editor& ed, const Window& win) {
  const auto& start = win.start();
  const auto& dim = win.dim();
  // first line is always the cmd prompt!
  int y = drawLine(start.y, lines[0].get(), ed, 0, win);
  if(!usingChoices()) return;
  int len = choices->size();
  int h = start.y + dim.y;
  const auto str = getStr();
  for(int idx=startLine;y<h&&idx<len;++idx) {
    const auto& line = choices->at(idx);
    if(!choices->match(line, str)) continue;
    y = drawLine(y, line, ed, idx, win);
  }
}

int CmdMsgBar::drawLine(int y, const std::string& line, Editor& ed,
                         int lineNum, const Window& win) {
  const auto& st = win.start();
  const auto& dim = win.dim();
  int xStart = st.x, wid = dim.x, start = 0;
  int len = (int)line.size();
  const auto* str = line.c_str();
  ULTRA_DEBUG("CmdMsgBar::drawLine: y=%d line=%s len=%d\n", y, line.c_str(),
              len);
  auto maxLen = std::max(len, wid);
  while(start < maxLen) {
    int diff = maxLen - start;
    int count = std::min(diff, wid);
    for(int i = 0; i < count; ++i) {
      auto c = start + i < len ? str[start + i] : ' ';
      // under the highlighted region
      auto highlighted = lineNum && lineNum == optLoc;
      AttrColor fg, bg;
      mode->getColorFor(fg, bg, lineNum, *(Buffer*)this, highlighted);
      ed.sendChar(xStart + i, y, fg, bg, c);
    }
    start += wid;
    ++y;
  }
  ULTRA_DEBUG("CmdMsgBar::drawLine: ended y=%d line=%s\n", y, line.c_str());
  return y;
}

std::string CmdMsgBar::getFinalChoice() const {
  if(usingChoices()) return choices->getFinalStr(optLoc, getStr());
  return getStr();
}

int CmdMsgBar::linesNeeded(const std::string& str, int wid) const {
  int len = (int)str.size();
  if(len <= 0) return 1;
  return (len + wid - 1) / wid;
}

int CmdMsgBar::totalLinesNeeded(const Point& dim) const {
  int count = at(0).numLinesNeeded(dim.x);
  if(!usingChoices()) return count;
  const auto str = getStr();
  for(int i=startLine;i<=optLoc;++i)
    count += linesNeeded(choices->at(i), dim.x);
  return count;
}

void CmdMsgBar::insert(const std::string& str) {
  lines[0].insert(str, cu.x);
  cu.x += (int)str.size();
}

// always insert on the first line!
void CmdMsgBar::insert(char c) {
  lines[0].insert(c, cu.x);
  ++cu.x;
  if(!usingChoices()) return;
  updateChoices();
  // then jump to the first matching option at this point!
  int len = choices->size();
  const auto str = getStr();
  for(int idx=startLine;idx<len;++idx) {
    if(choices->match(idx, str)) {
      optLoc = idx;
      startLine = idx;
      break;
    }
  }
  choices->updateMainBuffer(*this);
}

void CmdMsgBar::updateChoices() {
  if(!usingChoices()) return;
  if(choices->updateChoices(getStr())) {
    startLine = optLoc = 0;
  }
  choices->updateMainBuffer(*this);
}

void CmdMsgBar::clear() {
  auto& line = lines[cu.y];
  line.erase(0, line.length());
  cu = {0, 0};
  lineReset();
}

void CmdMsgBar::clearChoices() {
  choices = nullptr;
  optLoc = 0;
}

void CmdMsgBar::down() {
  if(!usingChoices()) return;
  int len = choices->size();
  const auto str = getStr();
  for(int idx=optLoc+1;idx<len;++idx) {
    if(choices->match(idx, str)) {
      optLoc = idx;
      break;
    }
  }
  choices->updateMainBuffer(*this);
}

void CmdMsgBar::up() {
  if(!usingChoices())
    return;
  const auto str = getStr();
  for(int idx=optLoc-1;idx>=0;--idx) {
    if(choices->match(idx, str)) {
      optLoc = idx;
      break;
    }
  }
  lineDown();
  choices->updateMainBuffer(*this);
}

void CmdMsgBar::lineUp(const Point& dim) {
  while(totalLinesNeeded(dim) > dim.y) ++startLine;
}

void CmdMsgBar::lineDown() { startLine = std::min(startLine, optLoc); }

} // end namespace teditor

