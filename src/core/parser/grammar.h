#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include "lexer.h"
#include <core/utils.h>

namespace teditor {
namespace parser {

struct LL1Table :
  public std::unordered_map<uint32_t, std::unordered_map<uint32_t, uint32_t>> {
};


class Grammar {
 public:
  /** pair to define a lexer token for this grammar */
  typedef std::pair<std::string, std::string> TerminalDef;
  /** pair to define a `lhs -> rhs` production rule for this grammar */
  typedef std::pair<std::string, Strings> NonTerminalDef;

  /**
   * @brief ctor
   * @param terms list of lexer tokens for this grammar
   * @param nonterms list of production rules for this grammar
   * @param startSym starting production rule for this grammar
   */
  Grammar(const std::vector<TerminalDef>& terms,
          const std::vector<NonTerminalDef>& nonterms,
          const std::string& startSym);

  /**
   * @brief Gets unique id of the input symbole
   * @param sym input symbol
   * @return unique id
   */
  uint32_t getId(const std::string& sym) const;

  /**
   * @brief Gets symbol name corresponding to the unique id
   * @param id unique id
   * @return symbol name
   */
  std::string getName(uint32_t id) const;

  /**
   * @defgroup IsTerminal
   * @{
   * @brief Checks if the input is a terminal or not
   * @param sym symbol name
   * @param id symbol's unique id
   * @return true if it is, else false
   */
  bool isTerminal(const std::string& sym) const;
  bool isTerminal(uint32_t id) const { return id < terminals.size(); }
  /** @} */

  /**
   * @defgroup GetProductionIds
   * @{
   * @brief Gets the list of production IDs for the given symbol
   * @param sym symbol name
   * @param id symbol's unique id
   * @return list of production IDs where this symbol is in LHS
   */
  const std::vector<uint32_t>& getProdIds(const std::string& sym) const;
  const std::vector<uint32_t>& getProdIds(uint32_t id) const;
  /** @} */

  /**
   * @brief Gets the start symbol for this grammar
   * @return the name of the symbol
   */
  const std::string& getStart() const { return start; }

 private:
  struct NonTerminal {
    uint32_t lhsId;
    Strings rhs;
    bool isEps() const { return rhs.empty(); }
  };

  std::unordered_map<std::string, uint32_t> nameToId;
  TokenDefs terminals;
  std::vector<NonTerminal> nonTerminals;
  Strings tNames;
  Strings ntNames;
  std::unordered_map<std::string, std::vector<uint32_t>> ntNameToProdIds;
  std::string start;

  void addTerminal(const std::string& name, const std::string& regex);
  void addNonTerminal(const std::string& name, const Strings& syms);
};  // class Grammar

}  // namespace parser
}  // namespace teditor
