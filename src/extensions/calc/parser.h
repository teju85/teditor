#pragma once

#include "core/parser/scanner.h"
#include "core/parser/lexer.h"
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

/** expression parser and evaluator */
struct Parser {
  void evaluate(const std::string& expr, VarMap& vars);

 private:
  bool lexingDone(const parser::Token& tok);
  Num64 computeBinaryOp(const Num64& a, const Num64& b, uint32_t op);
  Num64 computeUnaryFunc(const Num64& a, const std::string& func);
  bool isBinaryOp(uint32_t id);
  bool isUnaryFunc(uint32_t id);
  void evaluateExpr(parser::Scanner *sc, VarMap& vars, NumStack& stack);
};  // struct Parser

}  // namespace calc
}  // namespace teditor
