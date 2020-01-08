#include "core/parser/lexer.h"
#include "core/parser/scanner.h"
#include "core/parser/nfa.h"
#include "core/parser/regexs.h"
#include "catch.hpp"
#include <string>

namespace teditor {
namespace parser {

enum TokenIds {
  Int,
  Float,
  BrktOpen,
  BrktClose,
  SemiColon,
  Symbol,
  Operators,
  WhiteSpace,
};

#define TOKEN_CHECK_STR(lex, sc, st, en, typ)  do {        \
    auto tok = lex.next(&sc);                              \
    REQUIRE(tok.start == Point{st, 0});                    \
    REQUIRE(tok.end == Point{en, 0});                      \
    REQUIRE(tok.type == typ);                              \
  } while(0)

TEST_CASE("Lexer") {
  Lexer lex({{Float,      Regexs::FloatingPt},
             {Int,        Regexs::Integer},
             {BrktOpen,   "\\("},
             {BrktClose,  "\\)"},
             {SemiColon,  ";"},
             {Symbol,     Regexs::Variable},
             {Operators,  "[-+*/]"},
             {WhiteSpace, "\\s+"}});
  SECTION("simple-arithmetic") {
    std::string expr("1 + 2");
    StringScanner sc(expr);
    TOKEN_CHECK_STR(lex, sc, 0, 0, Int);
    TOKEN_CHECK_STR(lex, sc, 1, 1, WhiteSpace);
    TOKEN_CHECK_STR(lex, sc, 2, 2, Operators);
    TOKEN_CHECK_STR(lex, sc, 3, 3, WhiteSpace);
    TOKEN_CHECK_STR(lex, sc, 4, 4, Int);
    TOKEN_CHECK_STR(lex, sc, 4, 4, Token::End);
  }
}

} // end namespace parser
} // end namespace teditor
