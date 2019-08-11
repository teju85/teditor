#include "core/parser.h"
#include "catch.hpp"
#include "core/file_utils.h"
#include <string.h>
#include <string>

namespace teditor {

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
