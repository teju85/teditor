#pragma once

#include "pos2d.h"


namespace teditor {

class Buffer;
class Editor;

/** Window to draw the contents of an associated buffer */
class Window {
public:
  /** attach a buffer to this window */
  void attachBuff(Buffer& b);

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
  int drawLine(int y, const std::string& line, Editor& ed, int lineNum,
               const std::string& fg, const std::string& bg);
  /** @} */

  /** number of lines needed to draw the currrent buffer in this window */
  int totalLinesNeeded() const;

protected:
  Buffer* currBuff;
  Pos2di screenStart, screenDim;
};

}; // end namespace teditor
