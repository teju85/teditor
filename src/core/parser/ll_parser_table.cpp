#include "ll_parser_table.h"
#include "grammar.h"
#include <algorithm>
#include <core/utils.h>

namespace teditor {
namespace parser {

LLTableFirsts::LLTableFirsts(const Grammar& g): firstNT(), firstStrs(),
                                                epsId(g.getId(Grammar::Eps)),
                                                eofId(g.getId(Grammar::Eof)) {
  const auto nProds = g.numProductions();
  for (uint32_t i = 0; i < nProds; ++i) {
    std::vector<uint32_t> stack;
    LLTableFirsts::First prodFirst;
    auto lid = g.getLhs(i);
    const auto& rhs = g.getRhs(i);
    stack.push_back(lid);
    // take a union of all symbols in the RHS, while checking for 'eps'
    for (const auto& r : rhs) {
      auto rid = g.getId(r);
      const auto& f = getFirstFor(g, rid, stack);
      for (auto fi : f) prodFirst.insert(fi);
      // if FIRST(rhs) doesn't contain 'eps', then halt
      if (f.find(epsId) == f.end()) break;
    }
    // update FIRST(lhs)
    if (firstNT.find(lid) == firstNT.end()) firstNT[lid] = First();
    for (auto p : prodFirst) firstNT[lid].insert(p);
    // store the FIRST(rhs) for this production
    firstStrs.push_back(prodFirst);
    stack.pop_back();
  }
}

const LLTableFirsts::First& LLTableFirsts::getFirstFor(
  const Grammar& g, uint32_t id, std::vector<uint32_t>& stack) {
  auto stackItr = std::find(stack.begin(), stack.end(), id);
  if (stackItr != stack.end()) {
    std::string error = format("%s", g.getName(stack[0]).c_str());
    for (uint32_t i = 1; i < uint32_t(stack.size()); ++i)
      error += format(" -> %s", g.getName(stack[i]).c_str());
    ASSERT(false, "Left recursion found! %s", error.c_str());
  }
  // if FIRST(id) already exists, just return it
  const auto itr = firstNT.find(id);
  if (itr != firstNT.end()) return itr->second;
  LLTableFirsts::First myFirst;
  // else if it is a terminal, just return it
  if (g.isTerminal(id)) {
    myFirst.insert(id);
  } else {
    // else, compute it recursively via all of the production rules, where this
    // symbol occurs in lhs and get their union
    stack.push_back(id);
    const auto& pids = g.getProdIds(id);
    for (auto p : pids) {
      // take a union of FIRST of all symbols in the RHS
      for (const auto& rhs : g.getRhs(p)) {
        auto rid = g.getId(rhs);
        if (g.isTerminal(rid)) {
          myFirst.insert(rid);
          if (rid != epsId) break;  // 'eps' not found. So halt
          else continue;
        }
        const auto& f = getFirstFor(g, rid, stack);
        for (auto fi : f) myFirst.insert(fi);
        // if FIRST(rid) doesn't contain 'eps', then halt
        if (f.find(epsId) == f.end()) break;
      }
    }
    stack.pop_back();
  } // if (g.isTerminal...) ... else 
  firstNT[id] = myFirst;
  return firstNT.find(id)->second;
}

}  // namespace parser
}  // namespace teditor
