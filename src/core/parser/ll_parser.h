#pragma once

#include "grammar.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "ll_parser_table.h"

namespace teditor {
namespace parser {

struct LL_1 {
  LL_1(const Grammar& g);

 private:
  typedef std::unordered_map<uint32_t, uint32_t> TokenToProd;
  typedef std::unordered_map<uint32_t, TokenToProd> Table;

  Table table;
  std::shared_ptr<Lexer> lexer;

  void constructTable(const Grammar& g);
};  // struct LL_1

}  // namespace parser
}  // namespace teditor
