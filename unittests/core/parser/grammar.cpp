#include "core/parser/grammar.h"
#include "core/parser/regexs.h"
#include "catch.hpp"

namespace teditor {
namespace parser {

TEST_CASE("Grammar") {
  Grammar g;
  SECTION("case 1") {
    g.addTerminal("Int", Regexs::Integer);
    g.addTerminal("+", "[+]");
    g.addNonTerminal("Add", {"Int", "+", "Int"});
    REQUIRE(0 == g.getId("Int"));
    REQUIRE(1 == g.getId("+"));
    REQUIRE(2 == g.getId("Add"));
    REQUIRE_THROWS(g.getId("NotThere"));
    REQUIRE("Int" == g.getName(0));
    REQUIRE("+" == g.getName(1));
    REQUIRE("Add" == g.getName(2));
  }
}

} // end namespace parser
} // end namespace teditor
