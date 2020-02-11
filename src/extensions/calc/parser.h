#pragma once

#include "core/parser/scanner.h"
#include "core/parser/lexer.h"
#include "core/parser/grammar.h"
#include "core/parser/ll_parser.h"
#include <unordered_map>
#include "number.h"
#include <string>
#include <stack>
#include <vector>
#include <memory>

namespace teditor {
namespace calc {

typedef std::unordered_map<std::string, Num64> VarMap;
typedef std::stack<Num64> NumStack;
typedef Num64 (*UnaryFunc)(const Num64&);

/** expression parser and evaluator */
struct Parser {
  Parser();
  void evaluate(const std::string& expr, VarMap& vars);
  void evaluate(parser::Scanner *sc, VarMap& vars);

 private:
  parser::Grammar grammar;
  std::vector<UnaryFunc> unaries;

  bool lexingDone(const parser::Token& tok);
  Num64 computeBinaryOp(const Num64& a, const Num64& b, uint32_t op);
  Num64 computeUnaryFunc(const Num64& a, uint32_t funcId);
  bool isBinaryOp(uint32_t id);
  bool isUnaryFunc(uint32_t id);
  void evaluateExpr(std::shared_ptr<parser::Lexer>& lex, parser::Scanner *sc,
                    VarMap& vars, NumStack& stack);
};  // struct Parser

}  // namespace calc
}  // namespace teditor
