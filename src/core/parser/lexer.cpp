#include "scanner.h"
#include "nfa.h"
#include "lexer.h"

namespace teditor {
namespace parser {

const uint32_t Token::End = uint32_t(-1);
const uint32_t Token::Unknown = uint32_t(-2);
const uint32_t Token::Root = uint32_t(-3);


std::ostream& operator<<(std::ostream& os, const Token& tok) {
  if (tok.type == Token::End) {
    os << "type=END";
  } else if (tok.type == Token::Unknown) {
    os << "type=UNKNOWN";
  } else {
    os << "type=" << tok.type << " start=" << tok.start.y << "," << tok.start.x
       << " end=" << tok.end.y << "," << tok.end.x;
  }
  return os;
}

Lexer::Lexer(const TokenDefs& t): nfas(), tokenDefs(t), names() {
  for (auto td : tokenDefs) {
    if (!td.regex.empty()) {
      nfas.push_back(new NFA(td.regex));
      ASSERT(names.find(td.type) == names.end(),
             "Lexer: token id '%u' is already defined!", td.type);
      names[td.type] = td.name;
    }
  }
}

Lexer::~Lexer() { for (auto t : nfas) delete t; }

Token Lexer::next(Scanner* sc,
                  const std::unordered_set<uint32_t>& ignoreTypes) {
  auto tok = next(sc);
  while (ignoreTypes.find(tok.type) != ignoreTypes.end()) tok = next(sc);
  return tok;
}

Token Lexer::next(Scanner* sc, uint32_t ignoreType) {
  return next(sc, std::unordered_set<uint32_t>({ignoreType}));
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

const char* Lexer::token2str(uint32_t tok) const {
  if (tok == Token::End) return "End";
  if (tok == Token::Unknown) return "Unknown";
  if (tok == Token::Root) return "Root";
  const auto itr = names.find(tok);
  ASSERT(itr != names.end(), "token2str: Bad token type passed '%u'!", tok);
  return itr->second.c_str();
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
