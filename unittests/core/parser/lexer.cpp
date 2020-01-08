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
  Symbol
};

TEST_CASE("Lexer") {
  Lexer lex({{Float,     Regexs::FloatingPt},
             {Int,       Regexs::Integer},
             {BrktOpen,  "\\("},
             {BrktClose, "\\)"},
             {SemiColon, ";"},
             {Symbol,    Regexs::Variable}});
  StringScanner sc("1 + 2");
}

} // end namespace parser
} // end namespace teditor
