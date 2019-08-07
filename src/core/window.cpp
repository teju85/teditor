#include "buffer.h"
#include "editor.h"
#include "window.h"
#include "logger.h"

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

void Window::draw(Editor& ed) {
  getBuff().resize(screenStart, screenDim);
  getBuff().draw(ed);
}

void Window::drawCursor(Editor& ed, const std::string& bg) {
  getBuff().drawCursor(ed, bg);
}

int Window::totalLinesNeeded() const { return getBuff().totalLinesNeeded(); }


Windows::Windows(): wins(), currWin(1), borders() {
  // first window is always the cmBar window
  wins.push_back(new Window);
  // second window starts as the main window which can then further be split
  wins.push_back(new Window);
}

Windows::~Windows() {
  for(auto itr : wins) delete itr;
  wins.clear();
  borders.clear();
}

void Windows::incrementCurrWin() {
  ++currWin;
  if(currWin >= (int)wins.size()) currWin = 1;
}

void Windows::decrementCurrWin() {
  --currWin;
  if(currWin < 1) currWin = wins.size() - 1;
}

void Windows::draw(Editor& ed, bool cmdMsgBarActive) {
  DEBUG("draw: windows and cmdMsgBar\n");
  for(auto itr : wins) itr->draw(ed);
  if(cmdMsgBarActive) {
    DEBUG("draw: cmdMsgBar.drawCursor\n");
    int i = 0;
    for(auto itr : wins) {
      itr->drawCursor(ed, i == 0? "cursorbg" : "inactivecursorbg");
      ++i;
    }
  } else {
    DEBUG("draw: drawCursor\n");
    int i = 0;
    for(auto itr : wins) {
      if(i != 0)
        itr->drawCursor(ed, i == currWin? "cursorbg" : "inactivecursorbg");
      ++i;
    }
  }
  DEBUG("draw: drawing borders\n");
  for(auto& b : borders) {
    for(int i=b.sy;i<b.ey;++i) {
      ed.sendChar(b.x, i, "winframebg", "winframefg",
                  ed.getArgs().winSplitChar);
    }
  }
  DEBUG("draw: ended\n");
}

bool Windows::splitVertically() {
  ///@todo: support multiple splits
  if(wins.size() == 3) return false;
  auto* curr = wins[currWin];
  const auto& start = curr->start();
  const auto& dim = curr->dim();
  // -1 for the border
  int lWidth = (dim.x - 1) / 2;
  // right window
  int rWidth = dim.x - 1 - lWidth;
  Window* w = new Window;
  curr->resize(start, {lWidth, dim.y});
  w->setCurrBuff(curr->currBuffId());
  w->buffs = curr->buffs;
  w->resize({start.x+lWidth+1, start.y}, {rWidth, dim.y});
  wins.insert(wins.begin()+currWin+1, w);
  Border b = {start.y, start.y+dim.y, start.x + lWidth};
  borders.push_back(b);
  return true;
}

} // end namespace teditor
