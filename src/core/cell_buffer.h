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
  CellBuffer(size_t w = 0, size_t h = 0);
  void clear(AttrColor fg, AttrColor bg);
  void resize(size_t w, size_t h);
  const Cell& at(size_t x, size_t y) const { return cells[y*width+x]; }
  Cell& at(size_t x, size_t y) { return cells[y*width+x]; }
  size_t w() const { return width; }
  size_t h() const { return height; }

private:
  size_t width, height;
  std::vector<Cell> cells;
};

}; // end namespace teditor
