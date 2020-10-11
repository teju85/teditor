#include "parse_tree.h"

namespace teditor {
namespace parser {

const uint32_t Token::End = uint32_t(-1);
const uint32_t Token::Unknown = uint32_t(-2);
const uint32_t Token::Root = uint32_t(-3);


std::ostream& operator<<(std::ostream& os, const Token& tok) {
  if (tok.type == Token::End) {
    os << "type=END";
  } else if (tok.type == Token::Unknown) {
    os << "type=UNKNOWN";
  } else {
    os << "type=" << tok.type << " start=" << tok.start.y << "," << tok.start.x
       << " end=" << tok.end.y << "," << tok.end.x;
  }
  return os;
}

}  // namespace parser
}  // namespace teditor
