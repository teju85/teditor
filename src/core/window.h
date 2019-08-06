#pragma once

#include "pos2d.h"
#include <vector>


namespace teditor {

class Buffer;
class Buffers;
class Editor;

///@todo: have the concept of current-buffer moved from Editor to this class
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

  /**
   * @defgroup GetBuff
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
  void resize(const Pos2di& start, const Pos2di& dim);

  /**
   * @defgroup Coordinates convert buffer co-ords to screen and vice versa
   * @{
   */
  Pos2di buffer2screen(const Pos2di& loc) const;
  Pos2di screen2buffer(const Pos2di& loc) const;
  /** @} */

  /**
   * @defgroup Draw Functions to draw parts of the associated buffer
   * @{
   */
  void draw(Editor& ed);
  void drawCursor(Editor& ed, const std::string& bg);
  /** @} */

  /** number of lines needed to draw the currrent buffer in this window */
  int totalLinesNeeded() const;

  void incrementCurrBuff();
  void decrementCurrBuff();
  int currBuffId() const { return currBuff; }
  void setCurrBuff(int i) { currBuff = i; }

protected:
  Buffers* buffs;  // NOT owned by this class
  int currBuff;
  Pos2di screenStart, screenDim;
};

/** simple typedef of a list of windows */
typedef std::vector<Window*> Windows;

}; // end namespace teditor
