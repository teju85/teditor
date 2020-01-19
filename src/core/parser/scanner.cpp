#include "scanner.h"
#include "core/utils.h"
#include "core/file_utils.h"

namespace teditor {
namespace parser {

char StringScanner::next(Point& pt) {
  ASSERT(!isEof(), "next: called after EOF has been hit!");
  auto c = str[currPos];
  pt.x = Point::DataT(currPos);
  pt.y = 0;
  ++currPos;
  return c;
}

void StringScanner::rewind() {
  ASSERT(currPos > startPos,
         "rewind: called after hitting start! curr=%lu start=%lu", currPos,
         startPos);
  --currPos;
}

std::string StringScanner::at(const Point& begin, const Point& end) {
  std::string ret;
  if (begin.x <= end.x) ret = str.substr(begin.x, end.x - begin.x + 1);
  return ret;
}

BufferScanner::BufferScanner(Buffer& b): buf(b), currPos({0, 0}), end({0, 0}) {
  buf.end();
  end = buf.getPoint();
  buf.begin();
}

char BufferScanner::next(Point& pt) {
  ASSERT(!isEof(), "next: called after hitting EOF!");
  pt = currPos = buf.getPoint();
  auto& str = buf.at(pt.y).get();
  auto c = pt.x >= buf.lengthOf(pt.y) ? '\n' : str[pt.x];
  buf.right();
  return c;
}

std::string BufferScanner::at(const Point& begin, const Point& end) {
  auto out = buf.regionAsStr(begin, end);
  // Buffer::regionAsStr does not include 'end'!
  if (end.y < buf.length()) {
    auto& str = buf.at(end.y).get();
    auto c = end.x >= buf.lengthOf(end.y) ? '\n' : str[end.x];
    out += c;
  }
  return out;
}

}  // namespace parser
}  // namespace teditor
