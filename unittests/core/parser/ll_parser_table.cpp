#include "core/parser/ll_parser_table.h"
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
  LLTableFirsts first(g);
}

} // end namespace parser
} // end namespace teditor
