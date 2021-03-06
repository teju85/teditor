#pragma once

#include <cstdint>
#include "core/utils.h"
#include <ostream>
#include <vector>
#include "core/pos2d.h"

namespace teditor {
namespace parser {

/** Represents a token that is being generated by the Lexer */
struct Token {
  /**
   * token type. Custom parsers are free to define their own types for their
   * internal use. But from an interface perspective, it'll always be an integer
   * for faster lookup.
   */
  uint32_t type;
  /** start of this token in the input stream */
  Point start;
  /** end of this token in the input stream */
  Point end;

  bool isEof() const { return type == End; }

  /** special reserved token for denoting EOF */
  static const uint32_t End;
  /** special reserved token for denoting unknown token */
  static const uint32_t Unknown;
  /** special reserved token for denoting root node */
  static const uint32_t Root;

  friend std::ostream& operator<<(std::ostream& os, const Token& tok);
};  // struct Token

/** A node in the parse tree */
struct Node {
  Node(const Token& t) : tok(t), children() {}
  Node(uint32_t type, const Point& start, const Point& end);
  ~Node() { for (auto* child : children) delete child; }
  // ownership is taken over by this class
  void add(Node* n) { children.push_back(n); }
  void add(const Token& t) { add(new Node(t)); }
  void add(uint32_t type, const Point& start, const Point& end);
  size_t size() const { return children.size(); }
  bool isLeaf() const { return children.empty(); }
  Node& operator[](size_t idx) { return *children[idx]; }
  const Node& operator[](size_t idx) const { return *children[idx]; }

  Token tok;
  std::vector<Node*> children;

  friend std::ostream& operator<<(std::ostream& os, const Node& n);

 private:
  void printNode(std::ostream& os, int depth) const;
};  // struct Node

}  // namespace parser
}  // namespace teditor
