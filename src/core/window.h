#pragma once

#include "pos2d.h"
#include <vector>


namespace teditor {

class Buffer;
class Editor;

///@todo: have the concept of current-buffer moved from Editor to this class
/** Window to draw the contents of an associated buffer */
class Window {
public:
  ///@todo: use full Buffers reference instead of a single buffer
  /** attach a buffer to this window */
  void attachBuff(Buffer* b);

  /** returns the currently associated buffer with this window */
  Buffer& getCurrBuff();

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
  void drawBuffer(Editor& ed);
  void drawCursor(Editor& ed, const std::string& bg);
  void drawStatusBar(Editor& ed);
  /** @} */

  /** number of lines needed to draw the currrent buffer in this window */
  int totalLinesNeeded() const;

protected:
  Buffer* currBuff;
  Pos2di screenStart, screenDim;
};

/** simple typedef of a list of windows */
typedef std::vector<Window*> Windows;

}; // end namespace teditor
