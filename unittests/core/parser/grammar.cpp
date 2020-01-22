#include "core/parser/grammar.h"
#include "core/parser/regexs.h"
#include "catch.hpp"

namespace teditor {
namespace parser {

TEST_CASE("Grammar Construction") {
  Grammar g({
      {"Int", Regexs::Integer},
      {"+", "[+]"}
    }, {
      {"Add", {"Int", "+", "Int"}}
    },
    "Add");

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

  REQUIRE_THROWS(g.getProdIds("Int"));
  REQUIRE_THROWS(g.getProdIds("NotThere"));
  auto pids = g.getProdIds("Add");
  REQUIRE(1 == pids.size());
  REQUIRE(0 == pids[0]);
  pids = g.getProdIds(g.getId("Add"));
  REQUIRE(1 == pids.size());
  REQUIRE(0 == pids[0]);

  auto rhs = g.getRhs(0);
  REQUIRE(3 == rhs.size());
  REQUIRE("Int" == rhs[0]);
  REQUIRE("+" == rhs[1]);
  REQUIRE("Int" == rhs[2]);

  REQUIRE(1 == g.numNonTerminals());
  REQUIRE(1 == g.numProductions());
}

} // end namespace parser
} // end namespace teditor
