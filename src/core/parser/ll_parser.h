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
    constructTable();
  }

 private:
  typedef std::unordered_map<uint32_t, uint32_t> TokenToProd;
  typedef std::unordered_map<uint32_t, TokenToProd> Table;
  typedef std::unordered_map<uint32_t, std::unordered_set<uint32_t>> First;

  Table table;
  std::shared_ptr<Lexer> lexer;

  void constructTable() {
    First firsts;
  }
};  // struct LL<1>

}  // namespace parser
}  // namespace teditor
