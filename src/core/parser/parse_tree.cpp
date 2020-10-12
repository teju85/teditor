#include "parse_tree.h"

namespace teditor {
namespace parser {

const uint32_t Token::End = uint32_t(-1);
const uint32_t Token::Unknown = uint32_t(-2);
const uint32_t Token::Root = uint32_t(-3);


std::ostream& operator<<(std::ostream& os, const Token& tok) {
  if (tok.type == Token::End) {
    os << "End";
  } else if (tok.type == Token::Unknown) {
    os << "Unknown";
  } else if(tok.type == Token::Root) {
    os << "Root";
  } else {
    os << tok.type;
  }
  os << ": " << tok.start.y << "," << tok.start.x << " to " << tok.end.y << ","
     << tok.end.x;
  return os;
}

void Node::printNode(std::ostream& os, int depth) const {
  for (int i = 0; i < depth; ++i) {
    os << " |";
  }
  if (depth > 0) os << '-';
  os << '(' << tok << ")\n";
  if (!isLeaf()) {
    for (auto* child : children) {
      child->printNode(os, depth + 1);
    }
  }
}

std::ostream& operator<<(std::ostream& os, const Node& n) {
  n.printNode(os, 0);
  return os;
}

}  // namespace parser
}  // namespace teditor
