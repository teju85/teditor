#pragma once

#include <unordered_map>
#include <unordered_set>
#include <stdint.h>
#include <vector>

namespace teditor {
namespace parser {

class Grammar;

/**
 * @brief Contains FIRST set for all non-terminals in the current grammar
 */
struct LLTableFirsts {
  typedef std::unordered_set<uint32_t> First;
  typedef std::unordered_map<uint32_t, First> FirstMap;
  typedef std::vector<First> FirstArr;

  LLTableFirsts(const Grammar& g);

  FirstArr firstStrs;  // for all strings of symbols in production rules

 private:
  uint32_t epsId;
  uint32_t eofId;

  // recursive function with memoization for evaluating individual non-terminal
  // symbol's FIRST set
  const First& getFirstFor(const Grammar& g, uint32_t id, FirstMap& firstNT);
};  // struct LLTableFirst

}  // namespace parser
}  // namespace teditor