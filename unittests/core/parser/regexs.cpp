#include "core/parser/nfa.h"
#include "core/parser/regexs.h"
#include "catch.hpp"
#include <string>

namespace teditor {
namespace parser {

TEST_CASE("Regexs::FloatingPt") {
  NFA nfa(Regexs::FloatingPt);
  REQUIRE(nfa.find("1") == 0);
  REQUIRE(nfa.find("+1") == 1);
  REQUIRE(nfa.find("-1") == 1);
  REQUIRE(nfa.find("1.") == 1);
  REQUIRE(nfa.find("1.2") == 2);
  REQUIRE(nfa.find("1.2e-1") == 5);
  REQUIRE(nfa.find("1.2E-1") == 5);
  REQUIRE(nfa.find("1.2e+1") == 5);
  REQUIRE(nfa.find("1.2E+1") == 5);
  REQUIRE(nfa.find("1.2e1") == 4);
  REQUIRE(nfa.find("1.2E1") == 4);
  REQUIRE(nfa.find("-1.2e1") == 5);
  REQUIRE(nfa.find("-1.2E1") == 5);
  REQUIRE(nfa.find("-.2e1") == 4);
  REQUIRE(nfa.find("-.2") == 2);
  REQUIRE(nfa.find("hello") == NFA::NoMatch);
  REQUIRE(nfa.find("1ea") == 0);
  REQUIRE(nfa.find("a") == NFA::NoMatch);
  REQUIRE(nfa.find("A") == NFA::NoMatch);
  REQUIRE(nfa.find("z") == NFA::NoMatch);
  REQUIRE(nfa.find("Z") == NFA::NoMatch);
  REQUIRE(nfa.find("{") == NFA::NoMatch);
  REQUIRE(nfa.find("`") == NFA::NoMatch);
  REQUIRE(nfa.find("~") == NFA::NoMatch);
  REQUIRE(nfa.find("!") == NFA::NoMatch);
  REQUIRE(nfa.find("@") == NFA::NoMatch);
  REQUIRE(nfa.find("#") == NFA::NoMatch);
  REQUIRE(nfa.find("$") == NFA::NoMatch);
  REQUIRE(nfa.find("%") == NFA::NoMatch);
  REQUIRE(nfa.find("^") == NFA::NoMatch);
  REQUIRE(nfa.find("&") == NFA::NoMatch);
  REQUIRE(nfa.find("*") == NFA::NoMatch);
  REQUIRE(nfa.find("(") == NFA::NoMatch);
  REQUIRE(nfa.find(")") == NFA::NoMatch);
  REQUIRE(nfa.find("_") == NFA::NoMatch);
  REQUIRE(nfa.find("=") == NFA::NoMatch);
  REQUIRE(nfa.find("{") == NFA::NoMatch);
  REQUIRE(nfa.find("}") == NFA::NoMatch);
  REQUIRE(nfa.find("[") == NFA::NoMatch);
  REQUIRE(nfa.find("]") == NFA::NoMatch);
  REQUIRE(nfa.find("|") == NFA::NoMatch);
  REQUIRE(nfa.find("\\") == NFA::NoMatch);
  REQUIRE(nfa.find(":") == NFA::NoMatch);
  REQUIRE(nfa.find(";") == NFA::NoMatch);
  REQUIRE(nfa.find("\"") == NFA::NoMatch);
  REQUIRE(nfa.find("'") == NFA::NoMatch);
  REQUIRE(nfa.find("<") == NFA::NoMatch);
  REQUIRE(nfa.find(">") == NFA::NoMatch);
  REQUIRE(nfa.find(",") == NFA::NoMatch);
  REQUIRE(nfa.find("?") == NFA::NoMatch);
  REQUIRE(nfa.find("/") == NFA::NoMatch);
}

TEST_CASE("Regexs::Integer") {
  NFA nfa(Regexs::Integer);
  REQUIRE(nfa.find("1") == 0);
  REQUIRE(nfa.find("+1") == 1);
  REQUIRE(nfa.find("-1") == 1);
  REQUIRE(nfa.find("+125") == 3);
  REQUIRE(nfa.find("-3701") == 4);
  REQUIRE(nfa.find("abcd!") == NFA::NoMatch);
}

TEST_CASE("Regexs::HexInt") {
  NFA nfa(Regexs::HexInt);
  REQUIRE(nfa.find("0x1") == 2);
  REQUIRE(nfa.find("0X1") == 2);
  REQUIRE(nfa.find("0X1aA") == 4);
  REQUIRE(nfa.find("12345") == NFA::NoMatch);
}

TEST_CASE("Regexs::Variable") {
  NFA nfa(Regexs::Variable);
  REQUIRE(nfa.find("1") == NFA::NoMatch);
  REQUIRE(nfa.find("_a") == 1);
  REQUIRE(nfa.find("abc") == 2);
  REQUIRE(nfa.find("Abc") == 2);
}

}  // namespace parser
}  // namespace teditor
