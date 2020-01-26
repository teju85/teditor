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
  // auto start = g.getId(g.getStart());
  // follows[start] = {eofId};
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

// const First& LL_1::getFollowFor(const Grammar& g, uint32_t id,
//                                 const FirstMap& firsts, FirstMap& follows) {
//   // if FOLLOW(id) already exists, just return it
//   const auto itr = follows.find(id);
//   if (itr != follows.end()) return itr->second;
//   // else, compute it recursively via all of the production rules, where this
//   // symbol occurs in lhs and get their union
//   const auto& pids = g.getProdIds(id);
//   for (auto p : pids) {
//     const auto& rhs = g.getRhs(p);
//     if (rhs.size() >= 3) {
//       // FIRST(last_symbol) -> FOLLOW(last_but_one_symbol)
//       auto last = g.getId(rhs.back());
//       auto lastButOne = g.getId(rhs[rhs.size() - 2]);
//       if (follows.find(lastButOne) == follows.end())
//         follows[lastButOne] = First();
//       const auto& f = getFirstFor(g, last, firsts);
//       for (auto fi : f) {
//         if (fi != epsId) follows[lastButOne].insert(fi);
//       }
//       // FOLLOW(lhs) -> FOLLOW(rhs_last_but_one_symbol)
//       if (f.find(epsId) != f.end()) {
//         const auto& f= getFollowFor(g, lid, firsts, follows);
//         for (auto fi : f) follows[lastButOne].insert(fi);
//       }
//     }
//     // FOLLOW(lhs) -> FOLLOW(rhs_last_symbol)
//     if(rhs.size() >= 2) {
//       auto lastId = g.getId(rhs.back());
//       if (follows.find(lastId) == follows.end()) follows[lastId] = First();
//       const auto& f = getFollowFor(g, lid, firsts, follows);
//       for (auto fi : f) follows[lastId].insert(fi);
//     }
//   }
//   firsts[id] = myFirst;
//   return firsts.find(id)->second;
// }

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
    }
    prevLen = currLen;
    currLen = sizeof(followNT);
  }
}

}  // namespace parser
}  // namespace teditor
