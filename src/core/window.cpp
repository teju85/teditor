#include "buffer.h"
#include "editor.h"
#include "window.h"

namespace teditor {

void Window::attachBuff(Buffer* b) { currBuff = b; }

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

void Window::drawBuffer(Editor& ed) { currBuff->drawBuffer(ed); }

void Window::drawCursor(Editor& ed, const std::string& bg) {
  currBuff->drawCursor(ed, bg);
}

void Window::drawStatusBar(Editor& ed) { currBuff->drawStatusBar(ed); }

int Window::totalLinesNeeded() const { return currBuff->totalLinesNeeded(); }

} // end namespace teditor
