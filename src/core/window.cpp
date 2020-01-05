#include "buffer.h"
#include "editor.h"
#include "window.h"
#include "logger.h"
#include "terminal.h"
#include "option.h"

namespace teditor {

Window::Window(): buffs(nullptr), currBuff(0), screenStart(), screenDim() {
}

void Window::attachBuffs(Buffers* bs) { buffs = bs; }

Buffer& Window::getBuff() { return *(buffs->at(currBuff)); }
const Buffer& Window::getBuff() const { return *(buffs->at(currBuff)); }

void Window::incrementCurrBuff() {
  ++currBuff;
  if(currBuff >= buffs->size()) currBuff = 0;
}

void Window::decrementCurrBuff() {
  --currBuff;
  if(currBuff < 0) currBuff = buffs->size() - 1;
}

void Window::resize(const Point& start, const Point& dim) {
  screenStart = start;
  screenDim = dim;
}

void Window::draw(Editor& ed) {
  DEBUG("Window::draw started\n");
  auto& buff = getBuff();
  buff.lineUp(dim());
  buff.draw(ed, *this);
  DEBUG("Window::draw ended\n");
}

void Window::drawPoint(Editor& ed, const AttrColor& bg) {
  getBuff().drawPoint(ed, bg, *this);
}


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

void Windows::resize(size_t cmBarHt) {
  auto& term = Terminal::getInstance();
  Point sz(term.width(), term.height());
  sz.y -= cmBarHt;
  DEBUG("Windows::resize: buff-x,y=%d,%d ht=%d\n", sz.x, sz.y, cmBarHt);
  // cmbar
  wins[0]->resize({0, sz.y}, {sz.x, cmBarHt});
  // rest of the windows
  screenDim = sz;
  wins[1]->resize({0, 0}, sz);
  borders.clear();
  if(wins.size() == 2) return;
  ///@todo: update this to support arbitrary splits!
  // -1 for the border
  auto lWidth = (sz.x - 1) / 2;
  auto rWidth = sz.x - 1 - lWidth;
  auto start = wins[1]->start();
  wins[1]->resize(start, {lWidth, sz.y});
  wins[2]->resize({start.x+lWidth+1, start.y}, {rWidth, sz.y});
  Border b = {start.y, start.y+sz.y, start.x + lWidth};
  borders.push_back(b);
}

void Windows::incrementCurrWin() {
  ++currWin;
  if(currWin >= wins.size()) currWin = 1;
}

void Windows::decrementCurrWin() {
  --currWin;
  if(currWin < 1) currWin = wins.size() - 1;
}

void Windows::draw(Editor& ed, bool cmdMsgBarActive) {
  DEBUG("draw: windows and cmdMsgBar\n");
  for(auto itr : wins) itr->draw(ed);
  if(cmdMsgBarActive) {
    DEBUG("draw: cmdMsgBar.drawPoint\n");
    size_t i = 0;
    for(auto itr : wins) {
      const auto& bg = itr->getBuff().getColor("cursorbg");
      const auto& ibg = itr->getBuff().getColor("inactivecursorbg");
      itr->drawPoint(ed, i == 0? bg : ibg);
      ++i;
    }
  } else {
    DEBUG("draw: drawPoint\n");
    size_t i = 0;
    for(auto itr : wins) {
      DEBUG("draw: currWin=%d i=%d\n", currWin, i);
      if(i != 0) {
        const auto& bg = itr->getBuff().getColor("cursorbg");
        const auto& ibg = itr->getBuff().getColor("inactivecursorbg");
        itr->drawPoint(ed, i == currWin? bg : ibg);
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
    for (auto i = b.sy; i < b.ey; ++i)
      ed.sendChar(b.x, i, bg, fg, Option::get("windowSplitter").getChar());
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
  auto lWidth = (dim.x - 1) / 2;
  // right window
  auto rWidth = dim.x - 1 - lWidth;
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
  for (size_t i = 1; i < wins.size(); ++i) {
    delete wins[i];
    wins.erase(wins.begin() + i);
    --i;
  }
  borders.clear();
  wins.push_back(w);
  currWin = 1;
}

} // end namespace teditor
