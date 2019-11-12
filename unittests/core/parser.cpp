#include "core/parser.h"
#include "catch.hpp"
#include "core/file_utils.h"
#include <string.h>
#include <string>

namespace teditor {

TEST_CASE("Parser::Test - Long") {
  auto correct = slurp("samples/long.cpp");
  Parser p("cpp");
  Parser::Tree t;
  p.parse(correct, t);
  auto root = t.root();
  REQUIRE(root.childCount() == 4);
  REQUIRE(root.namedChildCount() == 4);
  REQUIRE(root.type() == "translation_unit");
  auto first = root[0];
  REQUIRE(first.childCount() == 2);
  REQUIRE(first.namedChildCount() == 1);
  REQUIRE(first.type() == "preproc_include");
  REQUIRE(first[0].type() == "system_lib_string");
  REQUIRE(first[0].childCount() == 0);
  REQUIRE(first[0].namedChildCount() == 0);
  auto func = root[2];
  REQUIRE(func.type() == "function_definition");
  REQUIRE(func.childCount() == 3);
  REQUIRE(func.namedChildCount() == 3);
  REQUIRE(func[0].childCount() == 0);
  REQUIRE(func[0].namedChildCount() == 0);
  REQUIRE(func[0].type() == "primitive_type");
  REQUIRE(func[1].childCount() == 2);
  REQUIRE(func[1].namedChildCount() == 2);
  REQUIRE(func[1].type() == "function_declarator");
  REQUIRE(func[1][0].childCount() == 0);
  REQUIRE(func[1][0].namedChildCount() == 0);
  REQUIRE(func[1][0].type() == "identifier");
  REQUIRE(func[1][1].childCount() == 3);
  REQUIRE(func[1][1].namedChildCount() == 1);
  REQUIRE(func[1][1].type() == "parameter_list");
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
