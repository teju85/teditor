#pragma once

#include "pos2d.h"
#include <vector>


namespace teditor {

class Buffer;
class Buffers;
class Editor;
class Windows;

/** Window to draw the contents of an associated buffer */
class Window {
public:
  Window();

  /**
   * @brief attach buffers list to this window. This list should be shared with
   *  all the other windows being displayed (except for the Window that wraps
   *  the CmdMsgBar)
   */
  void attachBuffs(Buffers* bs);

  /** copy the current window's buffers and ID to the new window */
  void copyTo(Window& w) { w.buffs = buffs;  w.currBuff = currBuff; }

  /**
   * @defgroup GetBuff Get buffer
   * @{
   * @brief returns the currently active buffer with this window
   */
  Buffer& getBuff();
  const Buffer& getBuff() const;
  /** @} */

  /**
   * @brief resize the window in case of window size change
   * @param start screen start location
   * @param dim screen dimension
   */
  void resize(const Point& start, const Point& dim);

  /**
   * @defgroup DrawWin Functions to draw parts of the associated buffer
   * @{
   */
  void draw(Editor& ed);
  void drawPoint(Editor& ed, const AttrColor& bg);
  /** @} */

  void incrementCurrBuff();
  void decrementCurrBuff();
  size_t currBuffId() const { return currBuff; }
  void setCurrBuff(size_t i) { currBuff = i; }

  const Point& start() const { return screenStart; }
  const Point& dim() const { return screenDim; }

protected:
  Buffers* buffs;  // NOT owned by this class
  size_t currBuff;
  Point screenStart, screenDim;
};


/** Vertical Border when splitting a window */
struct Border {
  size_t sy, ey, x;
};


/** a list of windows */
class Windows {
public:
  Windows();
  ~Windows();
  Window& getWindow() { return *wins[currWin]; }
  const Window& getWindow() const { return *wins[currWin]; }
  size_t currWinId() const { return currWin; }
  size_t size() const { return wins.size(); }
  void incrementCurrWin();
  void decrementCurrWin();
  Window* operator[](size_t idx) { return wins[idx]; }
  const Window* operator[](size_t idx) const { return wins[idx]; }
  void draw(Editor& ed, bool cmdMsgBarActive);
  /**
   * @brief split the window into 2 vertical parts
   * @return true if split was successful, else false
   */
  bool splitVertically();
  void clearAll();
  void resize(size_t cmBarHt);

private:
  std::vector<Window*> wins;
  size_t currWin;
  std::vector<Border> borders;
  Point screenDim;
};

}; // end namespace teditor
