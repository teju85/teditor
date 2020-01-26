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
  REQUIRE(15 == first.size(first.firstNT));
  SECTION("Terminals") {
    auto id = g.getId(Grammar::Eps);
    auto itr = first.firstNT.find(id);
    REQUIRE(itr != first.firstNT.end());
    auto set = itr->second;
    REQUIRE(1 == set.size());
    REQUIRE(set.end() != set.find(id));
    id = g.getId("(");
    itr = first.firstNT.find(id);
    REQUIRE(itr != first.firstNT.end());
    set = itr->second;
    REQUIRE(1 == set.size());
    REQUIRE(set.end() != set.find(id));
    id = g.getId("Int");
    itr = first.firstNT.find(id);
    REQUIRE(itr != first.firstNT.end());
    set = itr->second;
    REQUIRE(1 == set.size());
    REQUIRE(set.end() != set.find(id));
    id = g.getId("+");
    itr = first.firstNT.find(id);
    REQUIRE(itr != first.firstNT.end());
    set = itr->second;
    REQUIRE(1 == set.size());
    REQUIRE(set.end() != set.find(id));
    id = g.getId("*");
    itr = first.firstNT.find(id);
    REQUIRE(itr != first.firstNT.end());
    set = itr->second;
    REQUIRE(1 == set.size());
    REQUIRE(set.end() != set.find(id));
  }
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

TEST_CASE("FOLLOW") {
  Grammar g(getTokens(), getProds(), "E");
  LL_1::Firsts first(g);
  LL_1::Follows follow(g, first);
  REQUIRE(14 == follow.size(follow.followNT));
  SECTION("E") {
    auto itr = follow.followNT.find(g.getId("E"));
    REQUIRE(itr != follow.followNT.end());
    auto set = itr->second;
    REQUIRE(2 == set.size());
    REQUIRE(set.end() != set.find(g.getId(")")));
    REQUIRE(set.end() != set.find(g.getId(Grammar::Eof)));
  }
  SECTION("T") {
    auto itr = follow.followNT.find(g.getId("T"));
    REQUIRE(itr != follow.followNT.end());
    auto set = itr->second;
    REQUIRE(3 == set.size());
    REQUIRE(set.end() != set.find(g.getId("+")));
    REQUIRE(set.end() != set.find(g.getId(")")));
    REQUIRE(set.end() != set.find(g.getId(Grammar::Eof)));
  }
  SECTION("F") {
    auto itr = follow.followNT.find(g.getId("F"));
    REQUIRE(itr != follow.followNT.end());
    auto set = itr->second;
    REQUIRE(4 == set.size());
    REQUIRE(set.end() != set.find(g.getId("+")));
    REQUIRE(set.end() != set.find(g.getId(")")));
    REQUIRE(set.end() != set.find(g.getId("*")));
    REQUIRE(set.end() != set.find(g.getId(Grammar::Eof)));
  }
  SECTION("R") {
    auto itr = follow.followNT.find(g.getId("R"));
    REQUIRE(itr != follow.followNT.end());
    auto set = itr->second;
    REQUIRE(2 == set.size());
    REQUIRE(set.end() != set.find(g.getId(")")));
    REQUIRE(set.end() != set.find(g.getId(Grammar::Eof)));
  }
  SECTION("S") {
    auto itr = follow.followNT.find(g.getId("S"));
    REQUIRE(itr != follow.followNT.end());
    auto set = itr->second;
    REQUIRE(3 == set.size());
    REQUIRE(set.end() != set.find(g.getId("+")));
    REQUIRE(set.end() != set.find(g.getId(")")));
    REQUIRE(set.end() != set.find(g.getId(Grammar::Eof)));
  }
}

TEST_CASE("LL<1>") {
  Grammar g(getTokens(), getProds(), "E");
  LL_1 parser(g);
  REQUIRE(0 == parser("E", "Int"));
  REQUIRE(0 == parser("E", "("));
  REQUIRE(1 == parser("T", "Int"));
  REQUIRE(1 == parser("T", "("));
  REQUIRE(4 == parser("S", "+"));
  REQUIRE(5 == parser("S", "*"));
  REQUIRE(4 == parser("S", ")"));
  REQUIRE(4 == parser("S", "$"));
  REQUIRE(7 == parser("R", "+"));
  //REQUIRE(6 == parser("R", "*"));  ///@todo check this!
  REQUIRE(6 == parser("R", ")"));
  REQUIRE(6 == parser("R", "$"));
  REQUIRE(2 == parser("F", "Int"));
  REQUIRE(3 == parser("F", "("));
}

} // end namespace parser
} // end namespace teditor
