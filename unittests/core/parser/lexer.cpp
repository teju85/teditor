#include "core/parser/lexer.h"
#include "core/parser/scanner.h"
#include "core/parser/nfa.h"
#include "core/parser/regexs.h"
#include "catch.hpp"
#include <string>

namespace teditor {
namespace parser {

enum TokenIds {
  Float,
  Int,
  Assignment,
  BrktOpen,
  BrktClose,
  SemiColon,
  Symbol,
  Operators,
  WhiteSpace,
};

#define TOKEN_CHECK_STR(lex, sc, st, en, typ)  do {        \
    auto tok = lex.next(&sc);                              \
    REQUIRE(tok.type == typ);                              \
    REQUIRE(tok.start == st);                              \
    REQUIRE(tok.end == en);                                \
  } while(0)

#define CHECK_IGNORE(lex, sc, st, en, typ, ign)  do {      \
    auto tok = lex.next(&sc, ign);                         \
    REQUIRE(tok.type == typ);                              \
    REQUIRE(tok.start == st);                              \
    REQUIRE(tok.end == en);                                \
  } while(0)

TEST_CASE("Lexer") {
  Lexer lex({{Float,      Regexs::FloatingPt},
             {Int,        Regexs::Integer},
             {Assignment, "="},
             {BrktOpen,   "\\("},
             {BrktClose,  "\\)"},
             {SemiColon,  ";"},
             {Symbol,     Regexs::Variable},
             {Operators,  "[-+*/]"},
             {WhiteSpace, "\\s+"}});
  SECTION("simple-arithmetic") {
    std::string expr("1 + 2");
    StringScanner sc(expr);
    TOKEN_CHECK_STR(lex, sc, Point(0, 0), Point(0, 0), Int);
    TOKEN_CHECK_STR(lex, sc, Point(1, 0), Point(1, 0), WhiteSpace);
    TOKEN_CHECK_STR(lex, sc, Point(2, 0), Point(2, 0), Operators);
    TOKEN_CHECK_STR(lex, sc, Point(3, 0), Point(3, 0), WhiteSpace);
    TOKEN_CHECK_STR(lex, sc, Point(4, 0), Point(4, 0), Int);
    TOKEN_CHECK_STR(lex, sc, Point(-1, -1), Point(-1, -1), Token::End);
  }
  SECTION("simple-arithmetic-mixed-precision") {
    std::string expr("1 + 2.3");
    StringScanner sc(expr);
    TOKEN_CHECK_STR(lex, sc, Point(0, 0), Point(0, 0), Int);
    TOKEN_CHECK_STR(lex, sc, Point(1, 0), Point(1, 0), WhiteSpace);
    TOKEN_CHECK_STR(lex, sc, Point(2, 0), Point(2, 0), Operators);
    TOKEN_CHECK_STR(lex, sc, Point(3, 0), Point(3, 0), WhiteSpace);
    TOKEN_CHECK_STR(lex, sc, Point(4, 0), Point(6, 0), Float);
    TOKEN_CHECK_STR(lex, sc, Point(-1, -1), Point(-1, -1), Token::End);
  }
  SECTION("function-call") {
    std::string expr("sin(2)");
    StringScanner sc(expr);
    TOKEN_CHECK_STR(lex, sc, Point(0, 0), Point(2, 0), Symbol);
    TOKEN_CHECK_STR(lex, sc, Point(3, 0), Point(3, 0), BrktOpen);
    TOKEN_CHECK_STR(lex, sc, Point(4, 0), Point(4, 0), Int);
    TOKEN_CHECK_STR(lex, sc, Point(5, 0), Point(5, 0), BrktClose);
    TOKEN_CHECK_STR(lex, sc, Point(-1, -1), Point(-1, -1), Token::End);
  }
  SECTION("disable-print") {
    std::string expr("a = 123;");
    StringScanner sc(expr);
    TOKEN_CHECK_STR(lex, sc, Point(0, 0), Point(0, 0), Symbol);
    TOKEN_CHECK_STR(lex, sc, Point(1, 0), Point(1, 0), WhiteSpace);
    TOKEN_CHECK_STR(lex, sc, Point(2, 0), Point(2, 0), Assignment);
    TOKEN_CHECK_STR(lex, sc, Point(3, 0), Point(3, 0), WhiteSpace);
    TOKEN_CHECK_STR(lex, sc, Point(4, 0), Point(6, 0), Int);
    TOKEN_CHECK_STR(lex, sc, Point(7, 0), Point(7, 0), SemiColon);
    TOKEN_CHECK_STR(lex, sc, Point(-1, -1), Point(-1, -1), Token::End);
  }
  SECTION("disable-print no space") {
    std::string expr("a=123;");
    StringScanner sc(expr);
    TOKEN_CHECK_STR(lex, sc, Point(0, 0), Point(0, 0), Symbol);
    TOKEN_CHECK_STR(lex, sc, Point(1, 0), Point(1, 0), Assignment);
    TOKEN_CHECK_STR(lex, sc, Point(2, 0), Point(4, 0), Int);
    TOKEN_CHECK_STR(lex, sc, Point(5, 0), Point(5, 0), SemiColon);
    TOKEN_CHECK_STR(lex, sc, Point(-1, -1), Point(-1, -1), Token::End);
  }
  SECTION("negative number") {
    std::string expr("a = -123");
    StringScanner sc(expr);
    TOKEN_CHECK_STR(lex, sc, Point(0, 0), Point(0, 0), Symbol);
    TOKEN_CHECK_STR(lex, sc, Point(1, 0), Point(1, 0), WhiteSpace);
    TOKEN_CHECK_STR(lex, sc, Point(2, 0), Point(2, 0), Assignment);
    TOKEN_CHECK_STR(lex, sc, Point(3, 0), Point(3, 0), WhiteSpace);
    TOKEN_CHECK_STR(lex, sc, Point(4, 0), Point(7, 0), Int);
    TOKEN_CHECK_STR(lex, sc, Point(-1, -1), Point(-1, -1), Token::End);
  }
  SECTION("misinterpretation!") {
    std::string expr("a = -123 +456");
    StringScanner sc(expr);
    TOKEN_CHECK_STR(lex, sc, Point(0, 0), Point(0, 0), Symbol);
    TOKEN_CHECK_STR(lex, sc, Point(1, 0), Point(1, 0), WhiteSpace);
    TOKEN_CHECK_STR(lex, sc, Point(2, 0), Point(2, 0), Assignment);
    TOKEN_CHECK_STR(lex, sc, Point(3, 0), Point(3, 0), WhiteSpace);
    TOKEN_CHECK_STR(lex, sc, Point(4, 0), Point(7, 0), Int);
    TOKEN_CHECK_STR(lex, sc, Point(8, 0), Point(8, 0), WhiteSpace);
    // the '+' will be seen as attached to the number!
    TOKEN_CHECK_STR(lex, sc, Point(9, 0), Point(12, 0), Int);
    TOKEN_CHECK_STR(lex, sc, Point(-1, -1), Point(-1, -1), Token::End);
  }
  SECTION("simple-arithmetic with ignore whitespace") {
    std::string expr("1 + 2");
    StringScanner sc(expr);
    CHECK_IGNORE(lex, sc, Point(0, 0), Point(0, 0), Int, WhiteSpace);
    CHECK_IGNORE(lex, sc, Point(2, 0), Point(2, 0), Operators, WhiteSpace);
    CHECK_IGNORE(lex, sc, Point(4, 0), Point(4, 0), Int, WhiteSpace);
    CHECK_IGNORE(lex, sc, Point(-1, -1), Point(-1, -1), Token::End, WhiteSpace);
  }
}

#undef TOKEN_CHECK_STR
#undef CHECK_IGNORE

} // end namespace parser
} // end namespace teditor
