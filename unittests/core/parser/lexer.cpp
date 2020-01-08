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
  Lexer lex({{Int,       Regexs::Integer},
             {Float,     Regexs::FloatingPt},
             {BrktOpen,  "\\("},
             {BrktClose, "\\)"},
             {SemiColon, ";"},
             {Symbol,    Regexs::Variable}});
}

} // end namespace parser
} // end namespace teditor
