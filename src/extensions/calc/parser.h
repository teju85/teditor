#pragma once

#include "core/parser/scanner.h"
#include "core/parser/lexer.h"
#include <unordered_map>
#include "number.h"
#include <string>

namespace teditor {
namespace calc {

enum TokenIds {
  Float,
  Int,
  BrktOpen,
  BrktClose,
  Comma,
  SemiColon,
  Symbol,
  Plus,
  Minus,
  Multiply,
  Divide,
  Power,
  Assignment,
  Abs,
  Sine,
  Cosine,
  Tangent,
  ArcSince,
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
  WhiteSpace,
};  // enum TokenIds

typedef std::unordered_map<std::string, Num64> VarMap;

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

  parser::Lexer* lex;

  static const std::unordered_map<uint32_t, OpInfo> OpInfoMap;

  bool lexingDone(const parser::Token& tok);
  Num64 computeBinaryOp(const Num64& a, const Num64& b, TokenIds op);
};  // struct Parser

}  // namespace calc
}  // namespace teditor
