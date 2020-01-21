#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include "lexer.h"

namespace teditor {
namespace parser {

struct LL1Table :
  public std::unordered_map<uint32_t, std::unordered_map<uint32_t, uint32_t>> {
};

class Grammar {
 public:
  Grammar(): nameToId(), terminals(), nonTerminals(), tNames(), ntNames(),
             ntNameToProdIds(), start() {}

  void addTerminal(const std::string& name, const std::string& regex);
  void addNonTerminal(const std::string& name,
                      const std::vector<std::string>& syms);
  void markStart(const std::string& name) { start = name; }

  uint32_t getId(const std::string& name) const;
  std::string getName(uint32_t id) const;
  bool isTerminal(const std::string& sym) const;
  bool isTerminal(uint32_t id) const { return id < terminals.size(); }

 private:
  struct NonTerminal {
    uint32_t lhsId;
    std::vector<std::string> rhs;

    bool isEps() const { return rhs.empty(); }
  };  // struct NonTerminal

  std::unordered_map<std::string, uint32_t> nameToId;
  TokenDefs terminals;
  std::vector<NonTerminal> nonTerminals;
  std::vector<std::string> tNames;
  std::vector<std::string> ntNames;
  std::unordered_map<std::string, std::vector<uint32_t>> ntNameToProdIds;
  std::string start;
};  // class Grammar

}  // namespace parser
}  // namespace teditor
