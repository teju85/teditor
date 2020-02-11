// Thanks to:
//  https://eli.thegreenplace.net/2012/08/02/parsing-expressions-by-precedence-climbing

#include "parser.h"
#include "core/parser/regexs.h"

namespace teditor {
namespace calc {

Parser::Parser():
  grammar({
    {"IVal",  parser::Regexs::Integer},
    {"FVal",  parser::Regexs::FloatingPt},
    {"(",     "\\("},
    {")",     "\\)"},
    ///@todo: enable
    //{";",     ";"},
    {"Var",   parser::Regexs::Variable},
    // add all binary operators from here //
    {"=",     "="},
    {"+",     "[+]"},
    {"-",     "-"},
    {"*",     "[*]"},
    {"/",     "/"},
    ///@todo: enable
    //{"^",     "^"},
    // add all binary operators till here //
    // assumed to be all unary functions only!
    {"Func" , "[a-zA-Z_][a-zA-Z0-9_]*\\("},
    {"space", "\\s+"},
  }, {
    {"Num",  {"IVal"}},
    {"Num",  {"FVal"}},
    {"R",    {parser::Grammar::Eps}},
    {"S",    {parser::Grammar::Eps}},
    {"Fac",  {"Num"}},
    {"Term", {"Fac", "S"}},
    {"Expr", {"Term", "R"}},
    {"R",    {"+", "Expr"}},
    {"R",    {"-", "Expr"}},
    {"S",    {"*", "Term"}},
    {"S",    {"/", "Term"}},
    {"Fac",  {"(", "Expr", ")"}},
    {"Fac",  {"Func", "Expr", ")"}},
  },
    "Expr") {}

bool Parser::lexingDone(const parser::Token& tok) {
  return tok.type == parser::Token::End || tok.type == parser::Token::Unknown;
}

void Parser::evaluate(const std::string& expr, VarMap& vars) {
  parser::StringScanner sc(expr);
  evaluate(&sc, vars);
}

void Parser::evaluate(parser::Scanner *sc, VarMap& vars) {
  auto lex = grammar.getLexer();
  NumStack stack;
  evaluateExpr(lex, sc, vars, stack);
}

void Parser::evaluateExpr(std::shared_ptr<parser::Lexer>& lex,
                          parser::Scanner *sc, VarMap& vars, NumStack& stack) {
  auto tok = lex->nextWithIgnore(sc, grammar.getId("space"));
  if (lexingDone(tok)) return;
  //if (isNumber(tok.type))
  ///@todo:
}

Num64 Parser::computeBinaryOp(const Num64& a, const Num64& b, uint32_t id) {
  if (id == grammar.getId("+")) return a + b;
  if (id == grammar.getId("-")) return a - b;
  if (id == grammar.getId("*")) return a * b;
  if (id == grammar.getId("/")) return a / b;
  if (id == grammar.getId("^")) return pow(a, b);
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

bool Parser::isBinaryOp(uint32_t id) {
  return grammar.getId("=") <= id && id <= grammar.getId("^");
}

}  // namespace calc
}  // namespace teditor
