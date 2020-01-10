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
};  // struct Parser

}  // namespace calc
}  // namespace teditor
