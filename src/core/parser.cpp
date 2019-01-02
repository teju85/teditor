#include "parser.h"
#include <stdint.h>
#include "utils.h"


namespace teditor {

Parser::Parser(TSLanguage* lang): tsp(nullptr) {
    tsp = ts_parser_new();
    ts_parser_set_language(tsp, lang);
}

Parser::~Parser() {
    ts_parser_delete(tsp);
}


Tree::~Tree() {
    if(tree != nullptr) ts_tree_delete(tree);
}

void Tree::parse(const std::string& src, Parser& tsp) {
    TSTree* t = ts_parser_parse_string(tsp.get(), tree, src.c_str(),
                                       (uint32_t)src.size());
    if(tree != nullptr) {
        ts_tree_delete(tree);
        tree = nullptr;
    }
    tree = t;
}

void Tree::copy(Tree& out) const {
    ASSERT(out.tree == nullptr,
           "Tree::copy: Input tree has already been allocated!");
    out.tree = ts_tree_copy(tree);
}

} // end namespace teditor
