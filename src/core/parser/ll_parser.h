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
  LL(const Grammar& g): table(), lexer(g.getLexer()) {
    constructTable(g);
  }

 private:
  typedef std::unordered_map<uint32_t, uint32_t> TokenToProd;
  typedef std::unordered_map<uint32_t, TokenToProd> Table;
  typedef std::unordered_map<uint32_t, std::unordered_set<uint32_t>> First;

  Table table;
  std::shared_ptr<Lexer> lexer;

  void constructTable(const Grammar& g) {
    First firsts;
    auto nProds = g.numProductions();
    for (uint32_t i = 0; i < nProds; ++i) {
      for (const auto& rhs : g.getRhs(i)) {
      }
    }
  }
};  // struct LL<1>

}  // namespace parser
}  // namespace teditor
