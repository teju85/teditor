#include "scanner.h"
#include "nfa.h"
#include "lexer.h"

namespace teditor {
namespace parser {

const uint32_t Token::End = uint32_t(-1);
const uint32_t Token::Unknown = uint32_t(-2);
const uint32_t Token::Root = uint32_t(-3);

Lexer::Lexer(const TokenDefs& t): nfas(), tokenDefs(t) {
  for (auto td : tokenDefs) nfas.push_back(new NFA(td.regex));
}

Lexer::~Lexer() { for (auto t : nfas) delete t; }

Token Lexer::nextWithIgnore(Scanner* sc, uint32_t ignoreType) {
  auto tok = next(sc);
  while (tok.type == ignoreType) tok = next(sc);
  return tok;
}

Token Lexer::next(Scanner* sc) {
  bool first = true;
  Token ret;
  ret.type = Token::Unknown;
  ret.start = ret.end = Point{-1, -1};
  reset();
  while (!sc->isEof()) {
    Point pt;
    auto c = sc->next(pt);
    if (first) {
      ret.start = pt;
      ret.end = pt;
      first = false;
    }
    int nActives, nSoleMatches, nMatches;
    bool consumed = step(c, pt, nActives, nSoleMatches, nMatches);
    // no match!
    if (nActives <= 0) {
      ret.end = pt;
      ret.type = Token::Unknown;
      return ret;
    }
    if (consumed) continue;
    // all matching regex's have only one matching active states remaining
    // then pick the longest matching one
    if (nMatches == 0 && nSoleMatches > 0) {
      getLongestMatchingToken(ret, true);
      sc->rewind();
      return ret;
    }
    // there is possibility of match being found in next sequence of chars. So,
    // continue with the next char from scanner
  }
  ret.type = Token::End;
  // try to see if there are any matching states, if so, pick the longest one
  getLongestMatchingToken(ret, false);
  return ret;
}

bool Lexer::step(char c, const Point& pt, int& nActives, int& nSoleMatches,
                 int& nMatches) {
  bool consumed = false;
  nActives = nSoleMatches = nMatches = 0;
  int i = 0;
  for (auto n : nfas) {
    if (!n->areActiveStatesEmpty()) {
      consumed |= n->step(c, pt);
      if (!n->areActiveStatesEmpty()) ++nActives;
      if (n->isMatch(true)) ++nSoleMatches;
      else if (n->isMatch(false)) ++nMatches;
    }
    ++i;
  }
  return consumed;
}

void Lexer::getLongestMatchingToken(Token& ret, bool lastRemainingState) {
  int i = 0;
  for (auto n : nfas) {
    if (n->isMatch(lastRemainingState)) {
      const auto& mp = n->getMatchPos();
      // in case multiple tokens match at the same location, then give more
      // preference to the ones defined at the end of the tokenDefs list!
      if (mp >= ret.end) {
        ret.type = tokenDefs[i].type;
        ret.end = mp;
      }
    }
    ++i;
  }
}

void Lexer::reset() { for (auto n : nfas) n->reset(); }

}  // namespace parser
}  // namespace teditor
