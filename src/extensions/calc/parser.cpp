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
    {";",     ";"},
    {"Var",   parser::Regexs::Variable},
    // add all binary operators from here //
    {"=",     "="},
    {"+",     "[+]"},
    {"-",     "-"},
    {"*",     "[*]"},
    {"/",     "/"},
    {"^",     "^"},
    // add all binary operators till here //
    // add all unary functions from here //
    {"sq",    "sq\\("},
    {"cube",  "cube\\("},
    {"abs",   "abs\\("},
    {"sin",   "sin\\("},
    {"cos",   "cos\\("},
    {"tan",   "tan\\("},
    {"asin",  "asin\\("},
    {"acos",  "acos\\("},
    {"atan",  "atan\\("},
    {"sinh",  "sinh\\("},
    {"cosh",  "cosh\\("},
    {"tanh",  "tanh\\("},
    {"asinh", "asinh\\("},
    {"acosh", "acosh\\("},
    {"atanh", "atanh\\("},
    {"sqrt",  "sqrt\\("},
    {"cbrt",  "cbrt\\("},
    {"log",   "log\\("},
    {"log10", "log10\\("},
    {"exp",   "exp\\("},
    {"floor", "floor\\("},
    {"ceil",  "ceil\\("},
    {"round", "round\\("},
    {"int",   "int\\("},
    {"float", "float\\("},
    // add all unary functions till here //
    {"space", "\\s+"},
  }, {
    {"Num",  {"IVal"}},
    {"Num",  {"FVal"}},
    {"Add",  {"Num", "+", "Num"}},
    {"Sub",  {"Num", "-", "Num"}},
    {"Expr", {"Add"}},
    {"Expr", {"Sub"}},
    {"Expr", {"(", "Expr", ")"}},
  },
    "Expr"),
  unaries({
    sq,    cube,  abs,   sin,   cos,     tan,   asin,   acos,   atan,  sinh,
    cosh,  tanh,  asinh, acosh, atanh,   sqrt,  cbrt,   log,    log10, exp,
    floor, ceil,  round, toInt, toFloat,}) {
}

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

Num64 Parser::computeUnaryFunc(const Num64& a, uint32_t funcId) {
  auto id = funcId - grammar.getId("sq");
  if (id >= unaries.size()) return Num64();
  return unaries[id](a);
}

bool Parser::isBinaryOp(uint32_t id) {
  return grammar.getId("=") <= id && id <= grammar.getId("^");
}

bool Parser::isUnaryFunc(uint32_t id) {
  return grammar.getId("sq") <= id && id <= grammar.getId("float");
}

}  // namespace calc
}  // namespace teditor
