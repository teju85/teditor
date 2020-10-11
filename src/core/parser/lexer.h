#pragma once

#include <stdint.h>
#include <core/pos2d.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <ostream>
#include "core/utils.h"
#include "parse_tree.h"

namespace teditor {
namespace parser {


/** A regex string and a unique type to identify a token during lexing */
struct TokenDef {
  uint32_t type;
  std::string regex;
  std::string name;
};
typedef std::vector<TokenDef> TokenDefs;


class Scanner;
struct NFA;
/** Base lexing class for tokenizing the input stream */
struct Lexer {
  Lexer(const TokenDefs& t);
  virtual ~Lexer();
  virtual Token next(Scanner* sc);
  Token next(Scanner* sc, const std::unordered_set<uint32_t>& ignoreTypes);
  Token next(Scanner* sc, uint32_t ignoreType);
  const char* token2str(uint32_t tok) const;

 private:
  std::vector<NFA*> nfas;
  TokenDefs tokenDefs;
  std::unordered_map<uint32_t, std::string> names;

  void reset();
  bool step(char c, const Point& pt, int& nActives, int& nSoleMatches,
            int& nMatches);
  void getLongestMatchingToken(Token& ret, bool lastRemainingState);
};  // struct Lexer


}  // namespace parser
}  // namespace teditor
