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

void Window::draw(Editor& ed) {
  getBuff().draw(ed, *this);
}

void Window::drawCursor(Editor& ed, const AttrColor& bg) {
  getBuff().drawCursor(ed, bg, *this);
}

int Window::totalLinesNeeded() const { return getBuff().totalLinesNeeded(); }


Windows::Windows(): wins(), currWin(1), borders(), screenDim() {
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
      const auto& bg = itr->getBuff().getColor("cursorbg");
      const auto& ibg = itr->getBuff().getColor("inactivecursorbg");
      itr->drawCursor(ed, i == 0? bg : ibg);
      ++i;
    }
  } else {
    DEBUG("draw: drawCursor\n");
    int i = 0;
    for(auto itr : wins) {
      DEBUG("draw: currWin=%d i=%d\n", currWin, i);
      if(i != 0) {
        const auto& bg = itr->getBuff().getColor("cursorbg");
        const auto& ibg = itr->getBuff().getColor("inactivecursorbg");
        itr->drawCursor(ed, i == currWin? bg : ibg);
      }
      ++i;
    }
  }
  DEBUG("draw: drawing borders\n");
  // Note: This should still work with multiple windows, since these colors
  // are supposed to be universally defined for all modes
  const auto& fg = getWindow().getBuff().getColor("winframefg");
  const auto& bg = getWindow().getBuff().getColor("winframebg");
  for(auto& b : borders) {
    for(int i=b.sy;i<b.ey;++i)
      ed.sendChar(b.x, i, bg, fg, ed.getArgs().winSplitChar);
  }
  DEBUG("draw: ended\n");
}

bool Windows::splitVertically() {
  ///@todo: support multiple splits
  if(wins.size() == 3) return false;
  auto* curr = wins[currWin];
  const auto& start = curr->start();
  const auto& dim = curr->dim();
  screenDim = dim; // note down the current dim for a later 'clear' call
  // -1 for the border
  int lWidth = (dim.x - 1) / 2;
  // right window
  int rWidth = dim.x - 1 - lWidth;
  Window* w = new Window;
  curr->resize(start, {lWidth, dim.y});
  curr->copyTo(*w);
  w->resize({start.x+lWidth+1, start.y}, {rWidth, dim.y});
  wins.insert(wins.begin()+currWin+1, w);
  Border b = {start.y, start.y+dim.y, start.x + lWidth};
  borders.push_back(b);
  return true;
}

void Windows::clearAll() {
  auto* w = new Window;
  wins[currWin]->copyTo(*w);
  w->resize({0, 0}, screenDim);
  for(int i=1;i<(int)wins.size();++i) {
    delete wins[i];
    wins.erase(wins.begin() + i);
    --i;
  }
  borders.clear();
  wins.push_back(w);
  currWin = 1;
}

} // end namespace teditor
