#pragma once

#include "lexer.h"
#include <vector>

namespace teditor {
namespace parser {

/** Represents a node in the parse tree */
struct Node : public std::vector<Node*> {
  /** current node contents */
  Token token;
  ~Node();
  bool isLeaf() const { return empty(); }
  void addChild(const Token& t);
};  // struct Node

}  // namespace parser
}  // namespace teditor
