#pragma once

extern "C" {
#include <tree_sitter/runtime.h>
};
#include <string>


namespace teditor {

/** main parser class for dealing with parsing of buffers */
class Parser {
public:
    /**
     * @brief construct a parser object
     * @param lang the language
     */
    Parser(TSLanguage* lang);

    /** dtor */
    ~Parser();

    /** getter for the parser object */
    TSParser* get() { return tsp; }

private:
    /** the parser */
    TSParser* tsp;
};


///@todo: support TSInput for efficient parsing
/** Simple wrapper around TSTree for ease of use */
class Tree {
public:
    /** ctor */
    Tree(): tree(nullptr) {}

    /** dtor */
    ~Tree();

    /**
     * @brief parse the tree and get the tree constructed. This also takes care
     * of incremental parsing
     * @param src the input source string
     * @param tsp the parser
     */
    void parse(const std::string& src, Parser& tsp);

    /** copy the tree (mostly for thread-safe reasons) */
    void copy(Tree& out) const;

    /** getter for the tree object */
    TSTree* get() { return tree; }

private:
    /** the tree */
    TSTree* tree;
};

}; // end namespace teditor
