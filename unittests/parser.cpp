#include "core/parser.h"
#include "catch.hpp"
#include "core/utils.h"
#include <string.h>
#include <string>


extern "C" TSLanguage *tree_sitter_cpp();

namespace teditor {

TEST_CASE("Parser::Test - Correct") {
    auto correct = slurp("samples/correct.cpp");
    Parser p(tree_sitter_cpp());
    Tree t;
    t.parse(correct, p);
    TSNode root = ts_tree_root_node(t.get());
    REQUIRE(ts_node_child_count(root) == 1);
    std::string actual(ts_node_type(root));
    REQUIRE(actual == "translation_unit");
}

TEST_CASE("Parser::Test - Inorrect") {
    auto correct = slurp("samples/incorrect.cpp");
    Parser p(tree_sitter_cpp());
    Tree t;
    t.parse(correct, p);
    TSNode root = ts_tree_root_node(t.get());
    REQUIRE(ts_node_child_count(root) == 4);
    std::string actual(ts_node_type(root));
    REQUIRE(actual == "ERROR");
}

} // end namespace teditor
