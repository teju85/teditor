#include "ll_parser.h"
#include "grammar.h"
#include <algorithm>
#include <core/utils.h>

namespace teditor {
namespace parser {

LL_1::LL_1(const Grammar& g): table(), lexer(g.getLexer()) {
  constructTable(g);
}

void LL_1::constructTable(const Grammar& g) {
  LL_1::Firsts firsts(g);
  LL_1::Follows follows(g, firsts);
  const auto nProds = g.numProductions();
  for (uint32_t i = 0; i < nProds; ++i) {
    const auto& prodFirst = firsts.firstStrs[i];
    auto lid = g.getLhs(i);
    auto itr = table.find(lid);
    if (itr == table.end()) {
      table[lid] = TokenToProd();
      itr = table.find(lid);
    }
    for (const auto& tok : prodFirst) {
      auto itr2 = itr->second.find(tok);
      ASSERT(itr2 == itr->second.end(),
             "LL<1>: symbol=%u(%s) found a conflict in parseTable for"
             " token=%u(%s) at production=%u!", lid, g.getName(lid).c_str(),
             tok, g.getName(tok).c_str(), itr2->second);
      itr->second[tok] = i;
    }
    // // populate the parse table based on FOLLOW sets
    // if (prodFirst.find(epsId) != f.end()) {
    //   const auto& f = getFollowFor(g, lid, firsts, follows);
    //   for (const auto& tok : f) {
    //     auto itr2 = itr->second.find(tok);
    //     ASSERT(itr2 == itr->second.end(),
    //            "LL<1>: symbol=%u(%s) found a conflict in parseTable for"
    //            " token=%u(%s) at production=%u!", lid, g.getName(lid).c_str(),
    //            tok, g.getName(tok).c_str(), itr2->second);
    //     itr->second[tok] = i;
    //   }
    // }
  }
}

LL_1::Sets::Sets(const Grammar& g): epsId(g.getId(Grammar::Eps)),
                                    eofId(g.getId(Grammar::Eof)) {
}

bool LL_1::Sets::has(const LL_1::Sets::Set& f, uint32_t id) const {
  return f.find(id) != f.end();
}

bool LL_1::Sets::has(const LL_1::Sets::SetMap& f, uint32_t id) const {
  return f.find(id) != f.end();
}

void LL_1::Sets::add(LL_1::Sets::SetMap& sm, const LL_1::Sets::Set& s,
                     uint32_t id) const {
  auto itr = sm.find(id);
  if (itr == sm.end()) {
    sm.insert(std::make_pair(id, LL_1::Sets::Set()));
    itr = sm.find(id);
  }
  add(itr->second, s);
}

void LL_1::Sets::addExcept(SetMap& sm, const Set& s, uint32_t id,
                           uint32_t except) const {
  auto itr = sm.find(id);
  if (itr == sm.end()) {
    sm.insert(std::make_pair(id, LL_1::Sets::Set()));
    itr = sm.find(id);
  }
  addExcept(itr->second, s, except);
}

void LL_1::Sets::addExcept(Set& a, const Set& b, uint32_t except) const {
  for (auto bi : b) {
    if (bi != except) a.insert(bi);
  }
}

size_t LL_1::Sets::size(const SetMap& sm) const {
  size_t len = 0;
  for (const auto& itr : sm) len += itr.second.size();
  return len;
}

LL_1::Firsts::Firsts(const Grammar& g): LL_1::Sets(g), firstNT(), firstStrs() {
  const auto nProds = g.numProductions();
  for (uint32_t i = 0; i < nProds; ++i) {
    std::vector<uint32_t> stack;
    LL_1::Sets::Set prodFirst;
    auto lid = g.getLhs(i);
    const auto& rhs = g.getRhs(i);
    stack.push_back(lid);
    // take a union of all symbols in the RHS, while checking for 'eps'
    for (const auto& r : rhs) {
      auto rid = g.getId(r);
      const auto& f = getFirstFor(g, rid, stack);
      add(prodFirst, f);
      // if FIRST(rhs_i) doesn't contain 'eps', then halt
      if (!has(f, epsId)) break;
    }
    // update FIRST(lhs)
    add(firstNT, prodFirst, lid);
    // store the FIRST(rhs) for this production
    firstStrs.push_back(prodFirst);
    stack.pop_back();
  }
}

const LL_1::Sets::Set& LL_1::Firsts::getFirstFor(
  const Grammar& g, uint32_t id, std::vector<uint32_t>& stack) {
  auto stackItr = std::find(stack.begin(), stack.end(), id);
  if (stackItr != stack.end()) {
    std::string error;
    for (uint32_t i = 0; i < uint32_t(stack.size()); ++i)
      error += format("%s -> ", g.getName(stack[i]).c_str());
    error += format("%s", g.getName(id).c_str());
    ASSERT(false, "Left recursion found! %s", error.c_str());
  }
  // if FIRST(id) already exists, just return it
  const auto itr = firstNT.find(id);
  if (itr != firstNT.end()) return itr->second;
  LL_1::Sets::Set myFirst;
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
        add(myFirst, f);
        // if FIRST(rid) doesn't contain 'eps', then halt
        if (!has(f, epsId)) break;
      }
    }
    stack.pop_back();
  } // if (g.isTerminal...) ... else 
  firstNT[id] = myFirst;
  return firstNT.find(id)->second;
}

LL_1::Follows::Follows(const Grammar& g, const LL_1::Firsts& f):
  LL_1::Sets::Sets(g), followNT() {
  const auto nProds = g.numProductions();
  followNT[g.getStartId()] = {eofId};
  size_t prevLen = 0, currLen = size(followNT);
  while(prevLen != currLen) {
    for (uint32_t i = 0; i < nProds; ++i) {
      const auto& rhs = g.getRhs(i);
      auto lhs = g.getLhs(i);
      // FOLLOW(B) = FIRST(beta) for all productions: N -> alpha B beta
      LL_1::Sets::Set firsts;
      bool epsFoundSoFar = true;
      for (auto r = rhs.size() - 1; r > 0; --r) {
        auto rid = g.getId(rhs[r]);
        auto nextId = g.getId(rhs[r - 1]);
        if (g.isTerminal(rid)) {
          firsts.insert(rid);
          epsFoundSoFar = epsFoundSoFar && (rid == epsId);
        } else {
          auto itr = f.firstNT.find(rid);
          ASSERT(itr != f.firstNT.end(), "Follows: Unknown rhs=%s for lhs=%s!",
                 g.getName(rid).c_str(), g.getName(lhs).c_str());
          addExcept(firsts, itr->second, epsId);
          epsFoundSoFar = epsFoundSoFar && has(itr->second, epsId);
        }
        if (!g.isTerminal(nextId)) {
          addExcept(followNT, firsts, nextId, epsId);
          if (epsFoundSoFar) followNT[nextId].insert(eofId);
        }
      }
      // FOLLOW(B) = FOLLOW(N) for all productions: N -> alpha B
      auto rid = g.getId(rhs.back());
      auto litr = followNT.find(lhs);
      ASSERT(litr != followNT.end(), "Follows: unknown lhs=%s!",
             g.getName(lhs).c_str());
      // only if B is a non-terminal!
      if (!g.isTerminal(rid)) addExcept(followNT, litr->second, rid, epsId);
      // if there's a production N -> B alpha, and FIRST(alpha) contains eps
      if (epsFoundSoFar && rhs.size() > 1) {
        rid = g.getId(rhs.front());
        addExcept(followNT, litr->second, rid, epsId);
      }
    }  // for i = 0 ...
    prevLen = currLen;
    currLen = sizeof(followNT);
  }
}

}  // namespace parser
}  // namespace teditor
