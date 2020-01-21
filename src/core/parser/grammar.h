#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include "lexer.h"

namespace teditor {
namespace parser {

class Grammar {
 public:
  Grammar(): nameToId(), terminals(), nonTerminals(), start() {}

  bool isTerminal(const std::string& sym) const;

  void addTerminal(const std::string& name, const std::string& regex);
  void addNonTerminal(const std::string& name,
                      const std::vector<std::string>& syms);
  void markStart(const std::string& name);

 private:
  struct NonTerminal {
    uint32_t lhsId;
    std::vector<std::string> rhs;

    bool isEps() const { return rhs.empty(); }
  };  // struct NonTerminal

  std::unordered_map<std::string, std::vector<uint32_t>> nameToId;
  TokenDefs terminals;
  std::vector<NonTerminal> nonTerminals;
  uint32_t start;

  bool isTerminal(uint32_t id) const { return id < terminals.size(); }
};  // class Grammar

}  // namespace parser
}  // namespace teditor
