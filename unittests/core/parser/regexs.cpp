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
}

TEST_CASE("Regexs::Integer") {
  NFA nfa(Regexs::Integer);
  REQUIRE(nfa.find("1") == 0);
  REQUIRE(nfa.find("+1") == 1);
  REQUIRE(nfa.find("-1") == 1);
  REQUIRE(nfa.find("+125") == 3);
  REQUIRE(nfa.find("-3701") == 4);
}

}  // namespace parser
}  // namespace teditor
