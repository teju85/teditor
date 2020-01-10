// Thanks to:
//  https://eli.thegreenplace.net/2012/08/02/parsing-expressions-by-precedence-climbing

#include "parser.h"
#include "core/parser/regexs.h"

namespace teditor {
namespace calc {

const std::unordered_map<uint32_t, Parser::OpInfo> Parser::OpInfoMap = {
  {Assignment, {1, true}},
  {Plus,       {2, true}},
  {Minus,      {2, true}},
  {Multiply,   {3, true}},
  {Divide,     {3, true}},
  {Power,      {4, false}},
};

Parser::Parser() : lex(nullptr) {
  lex = new parser::Lexer({{Float,       parser::Regexs::FloatingPt},
                           {Int,         parser::Regexs::Integer},
                           {BrktOpen,    "\\("},
                           {BrktClose,   "\\)"},
                           {Comma,       ","},
                           {SemiColon,   ";"},
                           {Symbol,      parser::Regexs::Variable},
                           {Assignment,  "="},
                           {Plus,        "[+]"},
                           {Minus,       "[-]"},
                           {Multiply,    "[*]"},
                           {Divide,      "/"},
                           {Power,       "^"},
                           {Abs,         "abs"},
                           {Sine,        "sin"},
                           {Cosine,      "cos"},
                           {Tangent,     "tan"},
                           {ArcSince,    "asin"},
                           {ArcCosine,   "acos"},
                           {ArcTangent,  "atan"},
                           {SineH,       "sinh"},
                           {CosineH,     "cosh"},
                           {TangentH,    "tanh"},
                           {ArcSineH,    "asinh"},
                           {ArcCosineH,  "acosh"},
                           {ArcTangentH, "atanh"},
                           {Sqrt,        "sqrt"},
                           {Cbrt,        "cbrt"},
                           {Log,         "log"},
                           {Log10,       "log10"},
                           {Exp,         "exp"},
                           {Floor,       "floor"},
                           {Ceil,        "ceil"},
                           {Round,       "round"},
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

Num64 Parser::computeBinaryOp(const Num64& a, const Num64& b, TokenIds op) {
  switch(op) {
  case Plus:     return a + b;
  case Minus:    return a - b;
  case Multiply: return a * b;
  case Divide:   return a / b;
  case Power:    return pow(a, b);
  default:       return Num64();
  };
}

}  // namespace calc
}  // namespace teditor
