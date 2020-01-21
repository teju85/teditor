#include "core/parser/grammar.h"
#include "core/parser/regexs.h"
#include "catch.hpp"

namespace teditor {
namespace parser {

TEST_CASE("Grammar Construction") {
  Grammar g;
  g.addTerminal("Int", Regexs::Integer);
  g.addTerminal("+", "[+]");
  // duplicate terminals aren't allowed
  REQUIRE_THROWS(g.addTerminal("Int", Regexs::Integer));
  g.addNonTerminal("Add", {"Int", "+", "Int"});
  // can't add terminals after adding non-terminals!
  REQUIRE_THROWS(g.addTerminal("-", "[-]"));
  REQUIRE(0 == g.getId("Int"));
  REQUIRE(1 == g.getId("+"));
  REQUIRE(2 == g.getId("Add"));
  REQUIRE_THROWS(g.getId("NotThere"));
  REQUIRE("Int" == g.getName(0));
  REQUIRE("+" == g.getName(1));
  REQUIRE("Add" == g.getName(2));
  REQUIRE(g.isTerminal("Int"));
  REQUIRE(g.isTerminal("+"));
  REQUIRE_FALSE(g.isTerminal("Add"));
  REQUIRE_FALSE(g.isTerminal("NotThere"));
  REQUIRE(g.isTerminal(0));
  REQUIRE(g.isTerminal(1));
  REQUIRE_FALSE(g.isTerminal(2));
}

} // end namespace parser
} // end namespace teditor
