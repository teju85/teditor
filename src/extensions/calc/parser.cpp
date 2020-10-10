// Thanks to:
//  https://eli.thegreenplace.net/2012/08/02/parsing-expressions-by-precedence-climbing

#include "parser.h"
#include "core/parser/regexs.h"
#include "core/parser/scanner.h"
#include "core/parser/lexer.h"
#include "core/parser/grammar.h"

namespace teditor {
namespace calc {

parser::Grammar& getGrammar() {
  static parser::Grammar g({
    {"IVal",  parser::Regexs::Integer},
    {"FVal",  parser::Regexs::FloatingPt},
    {"(",     "\\("},
    {")",     "\\)"},
    ///@todo: enable
    //{";",     ";"},
    {"Var",   parser::Regexs::Variable},
    {"=",     "="},
    // add all binary operators from here //
    {"+",     "[+]"},
    {"-",     "-"},
    {"*",     "[*]"},
    {"/",     "/"},
    {"^",     "^"},
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
    {"rhs",  {"Term", "R"}},
    {"R",    {"+", "rhs"}},
    {"R",    {"-", "rhs"}},
    {"S",    {"*", "Term"}},
    {"S",    {"/", "Term"}},
    {"Fac",  {"(", "rhs", ")"}},
    {"Fac",  {"Func", "rhs", ")"}},
    {"Stmt", {"rhs"}},
    {"Stmt", {"lhs", "=", "rhs"}},
  },
    "Stmt");
  return g;
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
  auto& grammar = getGrammar();
  auto lex = grammar.getLexer();
  auto tok = lex->next(sc, grammar.getId("space"));
  if (lexingDone(tok)) return;
  //if (isNumber(tok.type))
  ///@todo:
}

Num64 Parser::computeBinaryOp(const Num64& a, const Num64& b, uint32_t id) {
  auto& grammar = getGrammar();
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

bool Parser::isUnaryFunc(uint32_t id) {
  auto& grammar = getGrammar();
  return id == grammar.getId("Func");
}

bool Parser::isBinaryOp(uint32_t id) {
  auto& grammar = getGrammar();
  return grammar.getId("+") <= id && id <= grammar.getId("^");
}

}  // namespace calc
}  // namespace teditor
