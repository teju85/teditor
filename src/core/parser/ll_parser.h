#pragma once

#include "grammar.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>

namespace teditor {
namespace parser {

struct LL_1 {
 private:
  struct Sets {
    typedef std::unordered_set<uint32_t> Set;
    typedef std::unordered_map<uint32_t, Set> SetMap;
    typedef std::vector<Set> SetArr;

    Sets(const Grammar& g);
    bool has(const Set& f, uint32_t id) const;
    bool has(const SetMap& f, uint32_t id) const;
    void add(SetMap& sm, const Set& s, uint32_t id) const;
    void add(Set& a, const Set& b) const { for (auto bi : b) a.insert(bi); }
    void addExcept(Set& a, const Set& b, uint32_t except) const;
    void addExcept(SetMap& sm, const Set& s, uint32_t id, uint32_t except) const;
    size_t size(const SetMap& sm) const;

   protected:
    uint32_t epsId, eofId;
  };

 public:
  /**
   * @brief Constructs the parser based on the input grammar
   * @param g input grammar
   */
  LL_1(const Grammar& g);

  /**
   * @brief Checks if there's an entry in the parse table for the current
   *        non-terminal and the terminal
   * @param sym the non-terminal symbol
   * @param tok terminal (aka token)
   * @return true if there is an element associated with this pair, else false
   */
  bool hasEntryFor(uint32_t sym, uint32_t tok) const;

  /**
   * @brief Gets the parse table entry for current non-terminal and terminal
   * @param sym the non-terminal symbol
   * @param tok terminal (aka token)
   * @return the production id (if there's no entry, an exception is raised)
   * @{
   */
  uint32_t operator()(uint32_t sym, uint32_t tok) const;
  uint32_t operator()(const std::string& sym, const std::string& tok) const;
  /** @} */

  /**
   * @brief Contains FIRST set for all non-terminals in the current grammar
   */
  struct Firsts : public Sets {
    Firsts(const Grammar& g);

    /** FIRST for every non-terminal in the grammar */
    SetMap firstNT;
    /** FIRST for every rhs in the productions in the grammar */
    SetArr firstStrs;

   private:
    // recursive function with memoization for evaluating individual
    // non-terminal symbol's FIRST set
    const Set& getFirstFor(const Grammar& g, uint32_t id,
                           std::vector<uint32_t>& stack);
  };  // struct Firsts

  /**
   * @brief Contains FOLLOW set for all non-terminals in the current grammar
   */
  struct Follows : public Sets {
    Follows(const Grammar& g, const Firsts& f);

    /** FOLLOW for every non-terminal in the grammar */
    SetMap followNT;
  };  // struct Follows

 private:
  typedef std::unordered_map<uint32_t, uint32_t> TokenToProd;
  typedef std::unordered_map<uint32_t, TokenToProd> Table;

  Table table;
  std::shared_ptr<Lexer> lexer;
  const Grammar& grammar;
  uint32_t epsId;

  void constructTable(const Grammar& g);
};  // struct LL_1

}  // namespace parser
}  // namespace teditor
