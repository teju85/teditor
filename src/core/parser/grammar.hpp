#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include "lexer.h"

namespace teditor {
namespace parser {

class Grammar {
 public:
  bool isTerminal(const std::string& sym) const;

 private:
  enum NtType {
    Sequence,
    OneOf,
    Epsilon,
  };  // enum NtType

  struct NonTerminal {
    NtType type;
    std::vector<std::string> symbols;
  };  // struct NonTerminal

  std::unordered_map<std::string, uint32_t> nameToId;
  TokenDefs terminals;
  std::vector<NonTerminal> nonTerminals;
};  // class Grammar

}  // namespace parser
}  // namespace teditor
