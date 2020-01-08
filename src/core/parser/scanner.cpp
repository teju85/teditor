#include "scanner.h"

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

void StringScanner::setStart(const Point& pt) {
  auto len = Point::DataT(str.size());
  ASSERT(pt.x < len,
         "setStart: point cannot be greater than string length! pt=%d len=%d",
         pt.x, len);
  startPos = currPos = pt.x;
}

void StringScanner::rewind() {
  ASSERT(currPos > startPos,
         "rewind: called after hitting start! curr=%lu start=%lu", currPos,
         startPos);
  --currPos;
}

}  // namespace parser
}  // namespace teditor
