#pragma once

extern "C" {
#include <tree_sitter/runtime.h>
};
#include <string>

namespace teditor {

/** Symbol as a numeric ID */
typedef TSSymbol SymId;

/** main parser class for dealing with parsing of buffers */
class Parser {
public:
  /**
   * @brief construct a parser object
   * @param lang the language
   */
  Parser(const std::string& lang);

  ~Parser() { ts_parser_delete(tsp); }

  class Tree;

  /**
   * @brief parse the input and get the tree constructed. This also takes care
   * of incremental parsing
   * @param src the input source string
   * @param tree the output tree
   */
  void parse(const std::string& src, Tree& tree);

  /** Simple wrapper around TSNode for ease of use */
  class Node {
  public:
    /** gets the child at the given index of the current node */
    Node operator[](int idx);

    /** node type as a numerical ID */
    SymId id() const { return ts_node_symbol(node); }

    /** node type as a string*/
    std::string type() const { return std::string(ts_node_type(node)); }

    /** number of child nodes of this node */
    int childCount() const { return ts_node_child_count(node); }

    /** returns the syntax-tree as an S-expression */
    std::string nodeString() const;

  private:
    /** the node */
    TSNode node;

    friend class Tree;
  }; // end class Node

  ///@todo: support TSInput for efficient parsing
  /** Simple wrapper around TSTree for ease of use */
  class Tree {
  public:
    Tree(): tree(nullptr) {}
    ~Tree() { if(tree != nullptr) ts_tree_delete(tree); }

    /** copy the tree (mostly to be used for thread-safe reasons) */
    void copy(Tree& out) const;

    /** root node of this tree */
    Node root();

  private:
    /** the tree */
    TSTree* tree;

    friend class Parser;
  }; // end class Tree

private:
  /** the parser */
  TSParser* tsp;
}; // end class Parser

}; // end namespace teditor
