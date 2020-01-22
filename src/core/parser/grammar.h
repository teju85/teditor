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


/**
 * @brief Definition of a non-terminal production in the grammar
 */
struct NonTerminal {
  /** unique id of the lhs non-terminal symbol of this production */
  uint32_t lhsId;
  /** list of terminal/non-terminal symbols in the rhs of this production */
  std::vector<std::string> rhs;

  /** whether the current production is an 'epsilon' or not */
  bool isEps() const { return rhs.empty(); }
};  // struct NonTerminal


class Grammar {
 public:
  typedef std::pair<std::string, std::string> TerminalDef;
  typedef std::pair<std::string, std::vector<std::string>> NonTerminalDef;

  Grammar(const std::vector<TerminalDef>& terms,
          const std::vector<NonTerminalDef>& nonterms,
          const std::string& startSym);

  uint32_t getId(const std::string& sym) const;
  std::string getName(uint32_t id) const;
  bool isTerminal(const std::string& sym) const;
  bool isTerminal(uint32_t id) const { return id < terminals.size(); }

 private:
  std::unordered_map<std::string, uint32_t> nameToId;
  TokenDefs terminals;
  std::vector<NonTerminal> nonTerminals;
  std::vector<std::string> tNames;
  std::vector<std::string> ntNames;
  std::unordered_map<std::string, std::vector<uint32_t>> ntNameToProdIds;
  std::string start;

  void addTerminal(const std::string& name, const std::string& regex);
  void addNonTerminal(const std::string& name,
                      const std::vector<std::string>& syms);
};  // class Grammar

}  // namespace parser
}  // namespace teditor
