#include "scanner.h"

namespace teditor {
namespace parser {

char StringScanner::nextChar() {
  auto c = str[currPos];
  ++currPos;
  return c;
}

}  // namespace parser
}  // namespace teditor
