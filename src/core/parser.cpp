#include "parser.h"
#include <stdint.h>
#include <map>
#include "utils.h"

//// List all languages supported here ////
extern "C" {
  TSLanguage* tree_sitter_cpp();
};
//// End of the list all languages supported ////

namespace teditor {

typedef TSLanguage* (*TSLangFunctor)();

TSLanguage* getTSLang(const std::string& lang) {
  //// Register all supported languages here ////
  static std::map<std::string, TSLangFunctor> langs = {
    {"cpp", tree_sitter_cpp},
  };
  //// End of the registry of all supported languages ////
  auto itr = langs.find(lang);
  ASSERT(itr != langs.end(), "Unsupported language: %s", lang.c_str());
  return itr->second();
}

Parser::Parser(const std::string& lang): tsp(nullptr) {
  tsp = ts_parser_new();
  ts_parser_set_language(tsp, getTSLang(lang));
}

void Parser::parse(const std::string& src, Tree& tree) {
  TSTree* t = ts_parser_parse_string(tsp, tree.tree, src.c_str(),
                                     (uint32_t)src.size());
  if(tree.tree != nullptr) {
    ts_tree_delete(tree.tree);
    tree.tree = nullptr;
  }
  tree.tree = t;
}

Parser::Node Parser::Node::operator[](int idx) {
  Node ret;
  ret.node = ts_node_named_child(node, idx);
  return ret;
}

std::string Parser::Node::nodeString() const {
  char *str = ts_node_string(node);
  std::string ret(str);
  free(str);
  return ret;
}

void Parser::Tree::copy(Tree& out) const {
  ASSERT(out.tree == nullptr,
         "Tree::copy: Input tree has already been allocated!");
  out.tree = ts_tree_copy(tree);
}

Parser::Node Parser::Tree::root() {
  Node ret;
  ret.node = ts_tree_root_node(tree);
  return ret;
}

} // end namespace teditor
