#pragma once

#include "core/parser/scanner.h"
#include "core/parser/lexer.h"
#include <unordered_map>
#include "number.h"
#include <string>
#include <stack>

namespace teditor {
namespace calc {

enum TokenIds {
  Float,
  Int,
  BrktOpen,
  BrktClose,
  SemiColon,
  Symbol,
  // add all binary operators from here //
  Assignment,
  Plus,
  Minus,
  Multiply,
  Divide,
  Power,
  // add all binary operators till here //
  // add all unary functions from here //
  Sq,
  Cube,
  Abs,
  Sine,
  Cosine,
  Tangent,
  ArcSine,
  ArcCosine,
  ArcTangent,
  SineH,
  CosineH,
  TangentH,
  ArcSineH,
  ArcCosineH,
  ArcTangentH,
  Sqrt,
  Cbrt,
  Log,
  Log10,
  Exp,
  Floor,
  Ceil,
  Round,
  ToInt,
  ToFloat,
  // add all unary functions till here //
  WhiteSpace,
};  // enum TokenIds

typedef std::unordered_map<std::string, Num64> VarMap;
typedef std::stack<Num64> NumStack;

/** expression parser and evaluator */
struct Parser {
  Parser();
  ~Parser() { delete lex; }

  void evaluate(const std::string& expr, VarMap& vars);
  void evaluate(parser::Scanner *sc, VarMap& vars);

 private:
  struct OpInfo {
    int precedence;
    bool leftAssoc;
  };

  std::unordered_map<uint32_t, OpInfo> OpInfoMap;
  parser::TokenDefs Tokens;
  parser::Lexer* lex;

  bool lexingDone(const parser::Token& tok);
  Num64 computeBinaryOp(const Num64& a, const Num64& b, TokenIds op);
  Num64 computeUnaryFunc(TokenIds funcId, const Num64& a);
  bool isBinaryOp(TokenIds id) { return (Assignment <= id) && (id <= Power); }
  bool isUnaryFunc(TokenIds id) { return (Sq <= id) && (id <= ToFloat); }
  bool isNumber(TokenIds id) { return id == Float || id == Int; }
  void evaluateExpr(parser::Scanner *sc, VarMap& vars, int precedence,
                    NumStack& stack);
};  // struct Parser

}  // namespace calc
}  // namespace teditor
