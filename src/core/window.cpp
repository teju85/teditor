#include "buffer.h"
#include "editor.h"
#include "window.h"

namespace teditor {

Window::Window(): buffs(nullptr), currBuff(0), screenStart(), screenDim() {
}

void Window::attachBuffs(Buffers* bs) { buffs = bs; }

Buffer& Window::getBuff() { return *(buffs->at(currBuff)); }
const Buffer& Window::getBuff() const { return *(buffs->at(currBuff)); }

void Window::incrementCurrBuff() {
  ++currBuff;
  if(currBuff >= (int)buffs->size()) currBuff = 0;
}

void Window::decrementCurrBuff() {
  --currBuff;
  if(currBuff < 0) currBuff = (int)buffs->size() - 1;
}

void Window::resize(const Pos2di& start, const Pos2di& dim) {
  screenStart = start;
  screenDim = dim;
}

Pos2di Window::buffer2screen(const Pos2di& loc) const {
  auto& buff = getBuff();
  int w = screenDim.x;
  Pos2di ret = screenStart;
  int relY = loc.y - buff.start();
  for(int idx=0;idx<relY;++idx)
    ret.y += buff.at(idx).numLinesNeeded(w);
  ret.y += (loc.x / w);
  ret.x += loc.x % w;
  return ret;
}

Pos2di Window::screen2buffer(const Pos2di& loc) const {
  auto& buff = getBuff();
  Pos2di rel = {loc.x - screenStart.x, loc.y - screenStart.y};
  Pos2di res = {0, buff.start()};
  int w = screenDim.x;
  int sy = 0;
  for(;sy<=rel.y;++res.y)
    sy += buff.at(res.y).numLinesNeeded(w);
  if(sy > rel.y) --res.y;
  int dely = rel.y - sy + buff.at(res.y).numLinesNeeded(w);
  res.x = dely * w + rel.x;
  return res;
}

void Window::draw(Editor& ed) { getBuff().draw(ed); }

void Window::drawCursor(Editor& ed, const std::string& bg) {
  getBuff().drawCursor(ed, bg);
}

int Window::totalLinesNeeded() const { return getBuff().totalLinesNeeded(); }


Windows::Windows(): wins(), currWin(1) {
  // first window is always the cmBar window
  wins.push_back(new Window);
  // second window starts as the main window which can then further be split
  wins.push_back(new Window);
}

Windows::~Windows() {
  for(auto itr : wins) delete itr;
  wins.clear();
}

void Windows::incrementCurrWin() {
  ++currWin;
  if(currWin >= (int)wins.size()) currWin = 1;
}

void Windows::decrementCurrWin() {
  --currWin;
  if(currWin < 1) currWin = wins.size() - 1;
}

} // end namespace teditor
