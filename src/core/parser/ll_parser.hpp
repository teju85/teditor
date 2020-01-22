#pragma once

#include "grammar.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace teditor {
namespace parser {

template <int k> struct LL;

template <>
struct LL<1> {
  LL(const Grammar& g): table(), lexer(g.getLexer()),
                        epsId(g.getId(Grammar::Eps)),
                        eofId(g.getId(Grammar::Eof)) {
    constructTable(g);
  }

 private:
  typedef std::unordered_map<uint32_t, uint32_t> TokenToProd;
  typedef std::unordered_map<uint32_t, TokenToProd> Table;

  Table table;
  std::shared_ptr<Lexer> lexer;
  uint32_t epsId;
  uint32_t eofId;

  typedef std::unordered_set<uint32_t> First;
  typedef std::unordered_map<uint32_t, First> FirstMap;

  void constructTable(const Grammar& g) {
    FirstMap firsts, follows;
    auto start = g.getId(g.getStart());
    follows[start] = {eofId};
    const auto nProds = g.numProductions();
    // construct FIRST and FOLLOW sets
    for (uint32_t i = 0; i < nProds; ++i) {
      First prodFirst;
      const auto& rhs = g.getRhs(i);
      // take a union of all symbols in the RHS, while checking for 'eps'
      for (const auto& r : rhs) {
        auto rid = g.getId(r);
        const auto& f = getFirstFor(g, rid, firsts);
        for (auto fi : f) prodFirst.insert(fi);
        // if FIRST(rhs) doesn't contain 'eps', then halt
        if (f.find(epsId) == f.end()) break;
      }
      // populate the parse table based on FIRST sets
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
      // populate the parse table based on FOLLOW sets
      if (prodFirst.find(epsId) != f.end()) {
        const auto& f = getFollowFor(g, lid, firsts, follows);
        for (const auto& tok : f) {
          auto itr2 = itr->second.find(tok);
          ASSERT(itr2 == itr->second.end(),
                 "LL<1>: symbol=%u(%s) found a conflict in parseTable for"
                 " token=%u(%s) at production=%u!", lid, g.getName(lid).c_str(),
                 tok, g.getName(tok).c_str(), itr2->second);
          itr->second[tok] = i;
        }
      }
    }
  }

  const First& getFirstFor(const Grammar& g, uint32_t id, FirstMap& firsts) {
    // if FIRST(id) already exists, just return it
    const auto itr = firsts.find(id);
    if (itr != firsts.end()) return itr->second;
    // else, compute it recursively via all of the production rules, where this
    // symbol occurs in lhs and get their union
    const auto& pids = g.getProdIds(id);
    First myFirst;
    for (auto p : pids) {
      // take a union of FIRST of all symbols in the RHS
      for (const auto& rhs : g.getRhs(p)) {
        auto rid = g.getId(rhs);
        if (g.isTerminal(rid)) {
          myFirst.insert(rid);
          continue;
        }
        const auto& f = getFirstFor(g, rid, firsts);
        for (auto fi : f) myFirst.insert(fi);
        // if FIRST(rid) doesn't contain 'eps', then halt
        if (f.find(epsId) == f.end()) break;
      }
    }
    firsts[id] = myFirst;
    return firsts.find(id)->second;
  }

  const First& getFollowFor(const Grammar& g, uint32_t id,
                            const FirstMap& firsts, FirstMap& follows) {
    // if FOLLOW(id) already exists, just return it
    const auto itr = follows.find(id);
    if (itr != follows.end()) return itr->second;
    // else, compute it recursively via all of the production rules, where this
    // symbol occurs in lhs and get their union
    const auto& pids = g.getProdIds(id);
    for (auto p : pids) {
      const auto& rhs = g.getRhs(p);
      if (rhs.size() >= 3) {
        // FIRST(last_symbol) -> FOLLOW(last_but_one_symbol)
        auto last = g.getId(rhs.back());
        auto lastButOne = g.getId(rhs[rhs.size() - 2]);
        if (follows.find(lastButOne) == follows.end())
          follows[lastButOne] = First();
        const auto& f = getFirstFor(g, last, firsts);
        for (auto fi : f) {
          if (fi != epsId) follows[lastButOne].insert(fi);
        }
        // FOLLOW(lhs) -> FOLLOW(rhs_last_but_one_symbol)
        if (f.find(epsId) != f.end()) {
          const auto& f= getFollowFor(g, lid, firsts, follows);
          for (auto fi : f) follows[lastButOne].insert(fi);
        }
      }
      // FOLLOW(lhs) -> FOLLOW(rhs_last_symbol)
      if(rhs.size() >= 2) {
        auto lastId = g.getId(rhs.back());
        if (follows.find(lastId) == follows.end()) follows[lastId] = First();
        const auto& f = getFollowFor(g, lid, firsts, follows);
        for (auto fi : f) follows[lastId].insert(fi);
      }

      // take a union of FIRST of all symbols in the RHS
      for (const auto& rhs : g.getRhs(p)) {
        auto rid = g.getId(rhs);
        if (g.isTerminal(rid)) {
          myFirst.insert(rid);
          continue;
        }
        const auto& f = getFollowFor(g, rid, firsts, follows);
        for (auto fi : f) myFirst.insert(fi);
        // if FIRST(rid) doesn't contain 'eps', then halt
        if (f.find(epsId) == f.end()) break;
      }
    }
    firsts[id] = myFirst;
    return firsts.find(id)->second;
  }
};  // struct LL<1>

}  // namespace parser
}  // namespace teditor
