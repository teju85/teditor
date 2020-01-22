#include "ll_parser_table.h"
#include "grammar.h"

namespace teditor {
namespace parser {

LLTableFirsts::LLTableFirsts(const Grammar& g): firstStrs(),
                                                epsId(g.getId(Grammar::Eps)),
                                                eofId(g.getId(Grammar::Eof)) {
  const auto nProds = g.numProductions();
  LLTableFirsts::FirstMap firstNT;  // for every non-terminal
  for (uint32_t i = 0; i < nProds; ++i) {
    LLTableFirsts::First prodFirst;
    const auto& rhs = g.getRhs(i);
    // take a union of all symbols in the RHS, while checking for 'eps'
    for (const auto& r : rhs) {
      auto rid = g.getId(r);
      const auto& f = getFirstFor(g, rid, firstNT);
      for (auto fi : f) prodFirst.insert(fi);
      // if FIRST(rhs) doesn't contain 'eps', then halt
      if (f.find(epsId) == f.end()) break;
    }
    firstStrs.push_back(prodFirst);
  }
}

const LLTableFirsts::First& LLTableFirsts::getFirstFor(
  const Grammar& g, uint32_t id, LLTableFirsts::FirstMap& firstNT) {
  // if FIRST(id) already exists, just return it
  const auto itr = firstNT.find(id);
  if (itr != firstNT.end()) return itr->second;
  // else, compute it recursively via all of the production rules, where this
  // symbol occurs in lhs and get their union
  const auto& pids = g.getProdIds(id);
  LLTableFirsts::First myFirst;
  for (auto p : pids) {
    // take a union of FIRST of all symbols in the RHS
    for (const auto& rhs : g.getRhs(p)) {
      auto rid = g.getId(rhs);
      if (g.isTerminal(rid)) {
        myFirst.insert(rid);
        continue;
      }
      const auto& f = getFirstFor(g, rid, firstNT);
      for (auto fi : f) myFirst.insert(fi);
      // if FIRST(rid) doesn't contain 'eps', then halt
      if (f.find(epsId) == f.end()) break;
    }
  }
  firstNT[id] = myFirst;
  return firstNT.find(id)->second;
}

}  // namespace parser
}  // namespace teditor