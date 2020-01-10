#include "parser.h"
#include "core/parser/regexs.h"

namespace teditor {
namespace calc {

Parser::Parser() : lex(nullptr) {
  lex = new parser::Lexer({{Float,      parser::Regexs::FloatingPt},
                           {Int,        parser::Regexs::Integer},
                           {BrktOpen,   "\\("},
                           {BrktClose,  "\\)"},
                           {Comma,      ","},
                           {SemiColon,  ";"},
                           {Symbol,     parser::Regexs::Variable},
                           {Plus,       "[+]"},
                           {Minus,      "[-]"},
                           {Multiply,   "[*]"},
                           {Divide,     "/"},
                           {Power,      "^"},
                           {Assignment, "="},
                           {WhiteSpace, "\\s+"}});
}

bool Parser::lexingDone(const parser::Token& tok) {
  return tok.type == parser::Token::End || tok.type == parser::Token::Unknown;
}

void Parser::evaluate(const std::string& expr, VarMap& vars) {
  parser::StringScanner sc(expr);
  evaluate(&sc, vars);
}

void Parser::evaluate(parser::Scanner *sc, VarMap& vars) {
  parser::Token tok;
  tok.type = parser::Token::End;
  do {
    tok = lex->next(sc);
    if (tok.type == WhiteSpace) continue;
  } while(!lexingDone(tok));
}

}  // namespace calc
}  // namespace teditor
