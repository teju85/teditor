#include "buffer.h"
#include "editor.h"
#include "window.h"

namespace teditor {

void Window::attachBuff(Buffer& b) { currBuff = &b; }

Buffer& Window::getCurrBuff() { return *currBuff; }

void Window::resize(const Pos2di& start, const Pos2di& dim) {
  screenStart = start;
  screenDim = dim;
}

Pos2di Window::buffer2screen(const Pos2di& loc) const {
  int w = screenDim.x;
  Pos2di ret = screenStart;
  int relY = loc.y - currBuff->start();
  for(int idx=0;idx<relY;++idx)
    ret.y += currBuff->at(idx).numLinesNeeded(w);
  ret.y += (loc.x / w);
  ret.x += loc.x % w;
  return ret;
}

Pos2di Window::screen2buffer(const Pos2di& loc) const {
  Pos2di rel = {loc.x - screenStart.x, loc.y - screenStart.y};
  Pos2di res = {0, currBuff->start()};
  int w = screenDim.x;
  int sy = 0;
  for(;sy<=rel.y;++res.y)
    sy += currBuff->at(res.y).numLinesNeeded(w);
  if(sy > rel.y) --res.y;
  int dely = rel.y - sy + currBuff->at(res.y).numLinesNeeded(w);
  res.x = dely * w + rel.x;
  return res;
}

void Window::drawBuffer(Editor& ed) {
  // last line is kept for status bar for this buffer
  int h = screenStart.y + screenDim.y - 1;
  int len = currBuff->length();
  for(int y=screenStart.y,idx=currBuff->start();y<h&&idx<len;++idx)
    y = drawLine(y, currBuff->at(idx).get(), ed, idx, "defaultfg", "defaultbg");
}

void Window::drawCursor(Editor& ed, const std::string& bg) {
  int n = currBuff->cursorCount();
  for(int i=0;i<n;++i) {
    const auto& culoc = currBuff->cursorAt(i);
    char c = currBuff->charAt(culoc);
    auto screenloc = buffer2screen(culoc);
    DEBUG("drawCursor: i=%d x,y=%d,%d sloc=%d,%d start=%d\n",
          i, culoc.x, culoc.y, screenloc.x, screenloc.y, currBuff->start());
    ed.sendChar(screenloc.x, screenloc.y, "cursorfg", bg, c);
  }
}

void Window::drawStatusBar(Editor& ed) {
  std::string line(screenDim.x, ' ');
  int x = screenStart.x;
  // last line is kept for status bar for this buffer
  int y = screenStart.x + screenDim.y - 1;
  ed.sendString(x, y, "statusfg", "statusbg", line.c_str(), screenDim.x);
  const auto& loc = currBuff->cursorAt(0);
  // modified + linenum
  int count = ed.sendStringf(x, y, "statusfg", "statusbg",
                             " %s [%d:%d]/%d ",
                             currBuff->isModified()? "**" : "  ",
                             loc.y, loc.x, currBuff->length());
  // buffer name
  count += ed.sendString(x+count, y, "statusnamefg", "statusbg",
                         currBuff->bufferName().c_str(),
                         (int)currBuff->bufferName().length());
  // buffer counts
  count += ed.sendStringf(x+count, y, "statusfg", "statusbg",
                          " <%d/%d> [%s]", ed.currBuffId()+1, ed.buffSize(),
                          currBuff->isRO()? "r-" : "rw");
  // multiple cursor counts
  if(currBuff->cursorCount() > 1) {
    count += ed.sendStringf(x+count, y, "statusfg", "statusbg",
                            " mc:%d", currBuff->cursorCount());
  }
  // mode
  count += ed.sendStringf(x+count, y, "statusfg", "statusbg",
                          " [mode=%s]", currBuff->modeName().c_str());
}

int Window::drawLine(int y, const std::string& line, Editor& ed, int lineNum,
                     const std::string& fg, const std::string& bg) {
  int xStart = screenStart.x;
  int wid = screenDim.x;
  int start = 0;
  int len = (int)line.size();
  // empty line
  if(len <= 0) return y + 1;
  const auto* str = line.c_str();
  bool isD = currBuff->modeName() == "dir";
  if(isD) {
    auto file = currBuff->dirModeGetFileAtLine(lineNum);
    file =  currBuff->getFileName() + '/' + file;
    isD = isDir(file.c_str());
  }
  while(start < len) {
    int diff = len - start;
    int count = std::min(diff, wid);
    for(int i=0;i<count;++i) {
      // under the highlighted region
      auto c = str[start + i];
      if(currBuff->getRegions().isInside(lineNum, start+i, currBuff->cursors()))
        ed.sendChar(xStart+i, y, "highlightfg", "highlightbg", c);
      else if(isD)
        ed.sendChar(xStart+i, y, "dirfg", bg, c);
      else
        ed.sendChar(xStart+i, y, fg, bg, c);
    }
    start += wid;
    ++y;
  }
  return y;
}

int Window::totalLinesNeeded() const {
  int end = currBuff->cursorAt(0).y;
  int len = 0;
  for(int i=currBuff->start();i<=end;++i)
    len += currBuff->at(i).numLinesNeeded(screenDim.x);
  return len;
}

} // end namespace teditor
