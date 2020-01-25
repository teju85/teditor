#include "core/parser/ll_parser.h"
#include "core/parser/grammar.h"
#include "core/parser/regexs.h"
#include "catch.hpp"

namespace teditor {
namespace parser {

// From: http://www.cs.ecu.edu/karl/5220/spr16/Notes/Top-down/first.html
struct GrammarDef {
  static const std::vector<Grammar::TerminalDef> Tokens;
  static const std::vector<Grammar::NonTerminalDef> Prods;
};

std::vector<Grammar::TerminalDef>& getTokens() {
  static std::vector<Grammar::TerminalDef> tokens = {
    {"Int", Regexs::Integer},
    {"+", "[+]"},
    {"*", "[*]"},
    {"(", "[(]"},
    {")", "[)]"},
  };
  return tokens;
}

std::vector<Grammar::NonTerminalDef>& getProds() {
  static std::vector<Grammar::NonTerminalDef> prods = {
    {"E", {"T", "R"}},
    {"T", {"F", "S"}},
    {"F", {"Int"}},
    {"F", {"(", "E", ")"}},
    {"S", {"eps"}},
    {"S", {"*", "T"}},
    {"R", {"eps"}},
    {"R", {"+", "E"}},
  };
  return prods;
}

TEST_CASE("FIRST") {
  Grammar g(getTokens(), getProds(), "E");
  LL_1::Firsts first(g);
  SECTION("E") {
    auto itr = first.firstNT.find(g.getId("E"));
    REQUIRE(itr != first.firstNT.end());
    auto set = itr->second;
    REQUIRE(2 == set.size());
    REQUIRE(set.end() != set.find(g.getId("Int")));
    REQUIRE(set.end() != set.find(g.getId("(")));
  }
  SECTION("T") {
    auto itr = first.firstNT.find(g.getId("T"));
    REQUIRE(itr != first.firstNT.end());
    auto set = itr->second;
    REQUIRE(2 == set.size());
    REQUIRE(set.end() != set.find(g.getId("Int")));
    REQUIRE(set.end() != set.find(g.getId("(")));
  }
  SECTION("F") {
    auto itr = first.firstNT.find(g.getId("F"));
    REQUIRE(itr != first.firstNT.end());
    auto set = itr->second;
    REQUIRE(2 == set.size());
    REQUIRE(set.end() != set.find(g.getId("Int")));
    REQUIRE(set.end() != set.find(g.getId("(")));
  }
  SECTION("R") {
    auto itr = first.firstNT.find(g.getId("R"));
    REQUIRE(itr != first.firstNT.end());
    auto set = itr->second;
    REQUIRE(2 == set.size());
    REQUIRE(set.end() != set.find(g.getId(Grammar::Eps)));
    REQUIRE(set.end() != set.find(g.getId("+")));
  }
  SECTION("S") {
    auto itr = first.firstNT.find(g.getId("S"));
    REQUIRE(itr != first.firstNT.end());
    auto set = itr->second;
    REQUIRE(2 == set.size());
    REQUIRE(set.end() != set.find(g.getId(Grammar::Eps)));
    REQUIRE(set.end() != set.find(g.getId("*")));
  }
}

} // end namespace parser
} // end namespace teditor
