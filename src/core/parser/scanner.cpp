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

}  // namespace parser
}  // namespace teditor
