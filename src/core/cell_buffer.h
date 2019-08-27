#pragma once

#include "colors.h"
#include <vector>


namespace teditor {

// a Unicode character
typedef uint32_t Chr;

/** a conceptual entity on the terminal screen */
struct Cell {
  Chr ch;
  AttrColor fg, bg;

  void set(Chr c, AttrColor f, AttrColor b);
  void set(Chr c, color_t f, color_t b);
  void copy(const Cell& from);
  int width() const;
};

bool operator==(const Cell& a, const Cell& b);


/** list of cells */
class CellBuffer {
public:
  CellBuffer(int w=0, int h=0);
  void clear(AttrColor fg, AttrColor bg);
  void resize(int w, int h);
  const Cell& at(int x, int y) const { return cells[y*width+x]; }
  Cell& at(int x, int y) { return cells[y*width+x]; }
  unsigned w() const { return (unsigned)width; }
  unsigned h() const { return (unsigned)height; }

private:
  int width, height;
  std::vector<Cell> cells;
};

}; // end namespace teditor
