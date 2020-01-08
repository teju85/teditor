#include "scanner.h"

namespace teditor {
namespace parser {

char StringScanner::nextChar(Point& pt) {
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
  currPos = pt.x;
}

}  // namespace parser
}  // namespace teditor
