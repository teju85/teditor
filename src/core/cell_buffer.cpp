#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for wcstring, strcasestr
#endif

#include "cell_buffer.h"
#include <wchar.h>


namespace teditor {

void Cell::copy(const Cell& from) {
  ch = from.ch;
  fg = from.fg;
  bg = from.bg;
}

void Cell::set(Chr c, AttrColor f, AttrColor b) {
  ch = c;
  fg = f;
  bg = b;
}

void Cell::set(Chr c, color_t f, color_t b) {
  ch = c;
  fg = f;
  bg = b;
}

int Cell::width() const {
  return wcwidth(ch);
}

bool operator==(const Cell& a, const Cell& b) {
  return (a.ch == b.ch) && (a.fg == b.fg) && (a.bg == b.bg);
}

CellBuffer::CellBuffer(size_t w, size_t h): width(w), height(h), cells(w * h) {
}

void CellBuffer::clear(AttrColor fg, AttrColor bg) {
  auto len = width * height;
  for (size_t i = 0; i < len; ++i) cells[i].set(' ', fg, bg);
}

void CellBuffer::resize(size_t w, size_t h) {
  if(width == w && height == h) return;
  cells.resize(w*h);
  width = w;
  height = h;
}

} // end namespace teditor
