#include "core/parser.h"
#include "catch.hpp"
#include "core/file_utils.h"
#include <string.h>
#include <string>

namespace teditor {

#define CHECK_NODE(n, count, t)              \
  REQUIRE(n.childCount() == count);          \
  REQUIRE(n.type() == t)

TEST_CASE("Parser::Test - Long") {
  auto correct = slurp("samples/long.cpp");
  Parser p("cpp");
  Parser::Tree t;
  p.parse(correct, t);
  auto root = t.root();
  CHECK_NODE(root, 4, "translation_unit");
  auto first = root[0];
  CHECK_NODE(first, 2, "preproc_include");
  CHECK_NODE(first[0], 0, "#include");
  CHECK_NODE(first[1], 0, "system_lib_string");
  auto func = root[3];
  CHECK_NODE(func, 3, "function_definition");
  CHECK_NODE(func[0], 0, "primitive_type");
  CHECK_NODE(func[1], 2, "function_declarator");
  CHECK_NODE(func[1][0], 0, "identifier");
  CHECK_NODE(func[1][1], 5, "parameter_list");
  CHECK_NODE(func[1][1][0], 0, "(");
  CHECK_NODE(func[1][1][1], 2, "parameter_declaration");
  CHECK_NODE(func[1][1][1][0], 0, "primitive_type");
  CHECK_NODE(func[1][1][1][1], 0, "identifier");
  CHECK_NODE(func[1][1][2], 0, ",");
  CHECK_NODE(func[1][1][3], 2, "parameter_declaration");
  CHECK_NODE(func[1][1][3][0], 0, "primitive_type");
  CHECK_NODE(func[1][1][3][1], 2, "pointer_declarator");
  CHECK_NODE(func[1][1][3][1][0], 0, "*");
  CHECK_NODE(func[1][1][3][1][1], 2, "pointer_declarator");
  CHECK_NODE(func[1][1][3][1][1][0], 0, "*");
  CHECK_NODE(func[1][1][3][1][1][1], 0, "identifier");
  CHECK_NODE(func[1][1][4], 0, ")");
  CHECK_NODE(func[2], 8, "compound_statement");
  CHECK_NODE(func[2][0], 0, "{");
  CHECK_NODE(func[2][1], 3, "declaration");
  CHECK_NODE(func[2][1][0], 0, "primitive_type");
  CHECK_NODE(func[2][1][1], 3, "init_declarator");
  CHECK_NODE(func[2][1][1][0], 0, "identifier");
  CHECK_NODE(func[2][1][1][1], 0, "=");
  CHECK_NODE(func[2][1][1][2], 0, "number_literal");
  CHECK_NODE(func[2][1][2], 0, ";");
  CHECK_NODE(func[2][2], 3, "declaration");
  CHECK_NODE(func[2][2][0], 0, "primitive_type");
  CHECK_NODE(func[2][2][1], 0, "identifier");
  CHECK_NODE(func[2][2][2], 0, ";");
  CHECK_NODE(func[2][3], 5, "declaration");
  CHECK_NODE(func[2][3][0], 0, "primitive_type");
  CHECK_NODE(func[2][3][1], 0, "identifier");
  CHECK_NODE(func[2][3][2], 0, ",");
  CHECK_NODE(func[2][3][3], 0, "identifier");
  CHECK_NODE(func[2][3][4], 0, ";");
}

TEST_CASE("Parser::Test - Correct") {
  auto correct = slurp("samples/correct.cpp");
  Parser p("cpp");
  Parser::Tree t;
  p.parse(correct, t);
  auto root = t.root();
  REQUIRE(root.childCount() == 1);
  REQUIRE(root.type() == "translation_unit");
}

TEST_CASE("Parser::Test - Inorrect") {
  auto incorrect = slurp("samples/incorrect.cpp");
  Parser p("cpp");
  Parser::Tree t;
  p.parse(incorrect, t);
  auto root = t.root();
  REQUIRE(root.childCount() == 4);
  REQUIRE(root.type() == "ERROR");
}

} // end namespace teditor
