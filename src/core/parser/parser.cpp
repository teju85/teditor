#include "parser.h"

namespace teditor {
namespace parser {

Node::~Node() { for (auto n : *this) delete n; }

void Node::addChild(const Token& t) {
  auto* n = new Node;
  n->token = t;
  push_back(n);
}

}  // namespace parser
}  // namespace teditor
