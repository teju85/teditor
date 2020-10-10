// Thanks to:
//  https://eli.thegreenplace.net/2012/08/02/parsing-expressions-by-precedence-climbing

#include "parser.h"
#include "core/parser/regexs.h"
#include "core/parser/scanner.h"
#include "core/parser/lexer.h"

namespace teditor {
namespace calc {

enum Tokens {
  IVal = 0,
  FVal,
  BrktOpen,
  BrktClose,
  Var,
  Equals,
  Plus,
  Minus,
  Mul,
  Div,
  Exp,
  Func,
  Space,
  SemiColon,
};  // enum Tokens

#define CASE(t) case t : return #t
const char* tokens2str(uint32_t tok) {
  if (tok == parser::Token::End) return "End";
  if (tok == parser::Token::Unknown) return "Unknown";
  switch (Tokens(tok)) {
    CASE(IVal);
    CASE(FVal);
    CASE(BrktOpen);
    CASE(BrktClose);
    CASE(Var);
    CASE(Equals);
    CASE(Plus);
    CASE(Minus);
    CASE(Mul);
    CASE(Div);
    CASE(Exp);
    CASE(Func);
    CASE(Space);
    CASE(SemiColon);
  default:
    ASSERT(false, "tokens2str: Bad token type passed '%d'!", tok);
  }
}
#undef CASE

parser::Lexer& getLexer() {
  static parser::Lexer lexer(
    {
      {IVal, parser::Regexs::Integer},
      {FVal, parser::Regexs::FloatingPt},
      {BrktOpen, "\\("},
      {BrktClose, "\\)"},
      {Var, parser::Regexs::Variable},
      {Equals, "="},
      // add all binary operators from here //
      {Plus, "[+]"},
      {Minus, "-"},
      {Mul, "[*]"},
      {Div, "/"},
      {Exp, "^"},
      // add all binary operators till here //
      // @todo: assumed to be all unary functions only for now!
      {Func , "[a-zA-Z_][a-zA-Z0-9_]*\\("},
      {Space, "\\s+"},
      {SemiColon,     ";"},
    });
  return lexer;
}

bool Parser::lexingDone(const parser::Token& tok) {
  return tok.type == parser::Token::End || tok.type == parser::Token::Unknown;
}

void Parser::evaluate(const std::string& expr, VarMap& vars) {
  parser::StringScanner sc(expr);
  NumStack stack;
  evaluateExpr(&sc, vars, stack);
}

void Parser::evaluateExpr(parser::Scanner *sc, VarMap& vars, NumStack& stack) {
  auto& lex = getLexer();
  auto tok = lex.next(sc, Space);
  if (lexingDone(tok)) return;
  //if (isNumber(tok.type))
  ///@todo:
}

Num64 Parser::computeBinaryOp(const Num64& a, const Num64& b, uint32_t id) {
  if (id == Plus) return a + b;
  if (id == Minus) return a - b;
  if (id == Mul) return a * b;
  if (id == Div) return a / b;
  if (id == Exp) return pow(a, b);
  return Num64();
}

Num64 Parser::computeUnaryFunc(const Num64& a, const std::string& func) {
  if ("sq(" == func)    return sq(a);
  if ("cube(" == func)  return cube(a);
  if ("abs(" == func)   return abs(a);
  if ("sin(" == func)   return sin(a);
  if ("cos(" == func)   return cos(a);
  if ("tan(" == func)   return tan(a);
  if ("asin(" == func)  return asin(a);
  if ("acos(" == func)  return acos(a);
  if ("atan(" == func)  return atan(a);
  if ("sinh(" == func)  return sinh(a);
  if ("cosh(" == func)  return cosh(a);
  if ("tanh(" == func)  return tanh(a);
  if ("asinh(" == func) return asinh(a);
  if ("acosh(" == func) return acosh(a);
  if ("atanh(" == func) return atanh(a);
  if ("sqrt(" == func)  return sqrt(a);
  if ("cbrt(" == func)  return cbrt(a);
  if ("log(" == func)   return log(a);
  if ("log10(" == func) return log10(a);
  if ("exp(" == func)   return exp(a);
  if ("floor(" == func) return floor(a);
  if ("ceil(" == func)  return ceil(a);
  if ("round(" == func) return round(a);
  if ("int(" == func)   return toInt(a);
  if ("float(" == func) return toFloat(a);
  return Num64();
}

bool Parser::isUnaryFunc(uint32_t id) { return id == Func; }

bool Parser::isBinaryOp(uint32_t id) {
  return Plus <= id && id <= Exp;
}

}  // namespace calc
}  // namespace teditor
