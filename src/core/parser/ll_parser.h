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

   protected:
    uint32_t epsId, eofId;
  };

 public:
  LL_1(const Grammar& g);

  /**
   * @brief Contains FIRST set for all non-terminals in the current grammar
   */
  struct Firsts : public Sets {
    typedef std::unordered_set<uint32_t> First;
    typedef std::unordered_map<uint32_t, First> FirstMap;
    typedef std::vector<First> FirstArr;

    Firsts(const Grammar& g);

    /** FIRST for every non-terminal in the grammar */
    FirstMap firstNT;
    /** FIRST for every rhs in the productions in the grammar */
    FirstArr firstStrs;

   private:
    uint32_t epsId;
    uint32_t eofId;

    // recursive function with memoization for evaluating individual
    // non-terminal symbol's FIRST set
    const First& getFirstFor(const Grammar& g, uint32_t id,
                             std::vector<uint32_t>& stack);
  };  // struct Firsts

  /**
   * @brief Contains FOLLOW set for all non-terminals in the current grammar
   */
  struct Follows {
    typedef Firsts::First Follow;
    typedef Firsts::FirstMap FollowMap;
    typedef Firsts::FirstArr FollowArr;

    Follows(const Grammar& g, const Firsts& f);

    /** FOLLOW for every non-terminal in the grammar */
    FollowMap followNT;

   private:
    uint32_t epsId;
    uint32_t eofId;
  };  // struct Follows

 private:
  typedef std::unordered_map<uint32_t, uint32_t> TokenToProd;
  typedef std::unordered_map<uint32_t, TokenToProd> Table;

  Table table;
  std::shared_ptr<Lexer> lexer;

  void constructTable(const Grammar& g);
};  // struct LL_1

}  // namespace parser
}  // namespace teditor
