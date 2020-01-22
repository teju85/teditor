#include "core/parser/grammar.h"
#include "core/parser/regexs.h"
#include "catch.hpp"

namespace teditor {
namespace parser {

TEST_CASE("Grammar Construction") {
  Grammar g({
      {"Int", Regexs::Integer},
      {"+", "[+]"},
      {"-", "-"},
    }, {
      {"Add", {"Int", "+", "Int"}},
      {"Sub", {"Int", "-", "Int"}},
      {"Expr", {"Add"}},
      {"Expr", {"Sub"}},
      {"Statement", {"Expr"}},
    },
    "Statement");

  REQUIRE(0 == g.getId("Int"));
  REQUIRE(1 == g.getId("+"));
  REQUIRE(2 == g.getId("-"));
  REQUIRE(3 == g.getId(Grammar::Eps));
  REQUIRE(4 == g.getId("Add"));
  REQUIRE(5 == g.getId("Sub"));
  REQUIRE(6 == g.getId("Expr"));
  REQUIRE(7 == g.getId("Statement"));
  REQUIRE_THROWS(g.getId("NotThere"));
  REQUIRE("Int" == g.getName(0));
  REQUIRE("+" == g.getName(1));
  REQUIRE("-" == g.getName(2));
  REQUIRE(Grammar::Eps == g.getName(3));
  REQUIRE("Add" == g.getName(4));
  REQUIRE("Sub" == g.getName(5));
  REQUIRE("Expr" == g.getName(6));
  REQUIRE("Statement" == g.getName(7));

  REQUIRE(g.isTerminal("Int"));
  REQUIRE(g.isTerminal("+"));
  REQUIRE(g.isTerminal("-"));
  REQUIRE(g.isTerminal(Grammar::Eps));
  REQUIRE_FALSE(g.isTerminal("Add"));
  REQUIRE_FALSE(g.isTerminal("Sub"));
  REQUIRE_FALSE(g.isTerminal("Expr"));
  REQUIRE_FALSE(g.isTerminal("Statement"));
  REQUIRE_FALSE(g.isTerminal("NotThere"));
  REQUIRE(g.isTerminal(0));
  REQUIRE(g.isTerminal(1));
  REQUIRE(g.isTerminal(2));
  REQUIRE(g.isTerminal(3));
  REQUIRE_FALSE(g.isTerminal(4));
  REQUIRE_FALSE(g.isTerminal(5));
  REQUIRE_FALSE(g.isTerminal(6));
  REQUIRE_FALSE(g.isTerminal(7));

  REQUIRE("Statement" == g.getStart());

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

  REQUIRE(4 == g.numNonTerminals());
  REQUIRE(5 == g.numProductions());
}

void badGrammar() {
  Grammar g({
      {"Int", Regexs::Integer},
      {"+", "[+]"},
      {"-", "-"},
    }, {
      {"Add", {"Int", "+", "Int"}},
      {"Sub", {"Int", "-", "Int"}},
      {"Expr", {"Add"}},
      {"-", {"Sub"}},   // existing terminal symbol used!!
      {"Statement", {"Expr"}},
    },
    "Statement");
}

TEST_CASE("Bad Grammar") {
  REQUIRE_THROWS(badGrammar());
}

} // end namespace parser
} // end namespace teditor
