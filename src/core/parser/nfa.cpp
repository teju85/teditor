#include "nfa.h"
#include <core/utils.h>

namespace teditor {
namespace parser {

const size_t NFA::NoMatch = std::string::npos;

bool NFA::State::isMatch(char in) const {
  switch(c) {
  case Split:
  case Match:         return false;
  case Digit:         return in >= '0' && in <= '9';
  case WhiteSpace:    return in == ' ' || in == '\t';
  case NonWhiteSpace: return in != ' ' && in != '\t';
  case Any:           return true;
  case AnyFromList:   return s.find(in) != std::string::npos;
  case NoneFromList:  return s.find(in) == std::string::npos;
  default:            return (c & 0xff) == in;
  };
}

NFA::Fragment::Fragment(State* e): entry(e), tails() { appendState(e); }

void NFA::Fragment::addState(State* s) {
  for (auto t : tails) t->next = s;
  tails.clear();
  appendState(s);
}


NFA::CompilerState::CompilerState() :
  prevBackSlash(false), prevSqBracketOpen(false), isUnderRange(false),
  isUnderSqBracket(false) {
}

void NFA::CompilerState::validate(const std::string& reg) {
  if (prevBackSlash)
    ASSERT(false, "No succeeding char after backslash? regex=%s", reg.c_str());
  if (prevSqBracketOpen)
    ASSERT(false, "No succeeding char after '['? regex=%s", reg.c_str());
  if (isUnderRange)
    ASSERT(false, "No succeeding char after '-'? regex=%s", reg.c_str());
  if (isUnderSqBracket)
    ASSERT(false, "No matching ']' char for '['? regex=%s", reg.c_str());
}

NFA::NFA(const std::string& reg) : regex(reg) {
  CompilerState cState;
  for (auto c : reg) parseChar(c, cState);
  cState.validate(reg);
  stitchFragments();
  // reached the end of the list, note down its start state and add match state
  ASSERT(fragments.size() == 1,
         "After stitching, there should only be one fragment left! [%lu]",
         fragments.size());
  auto& frag = fragments.top();
  matchState = createState(Specials::Match);
  frag.addState(matchState);
  startState = frag.entry;
  fragments.pop();
}

size_t NFA::find(const std::string& str, size_t start, size_t end) {
  if (end == 0) end = str.size();
  reset();
  for (; start < end; ++start) {
    step(str[start], start);
    if (areActiveStatesEmpty()) return NoMatch;
    // if only match state remains, no further progress is possible
    if (isMatch(true)) return getMatchPos();
  }
  return getMatchPos();
}

void NFA::reset() {
  matchState->matchPos = NoMatch;
  acs.current().clear();
  acs.current().insert(startState);
  stepThroughSplitStates();
}

void NFA::step(char c, size_t pos) {
  acs.next().clear();
  for (auto& a : acs.current()) {
    auto& next = acs.next();
    if (a->c == Specials::Match) {
      next.insert(a);
      continue;
    }
    if (a->isMatch(c)) {
      if (a->next != nullptr) {
        a->next->matchPos = pos;
        next.insert(a->next);
      }
      if (a->other != nullptr) {
        a->other->matchPos = pos;
        next.insert(a->other);
      }
    }
  }
  acs.update();
  stepThroughSplitStates();
}

bool NFA::isMatch(bool lastStateRemaining) const {
  // this is useful to prematurely terminate the main loop
  if (lastStateRemaining && acs.current().size() != 1) return false;
  const auto itr = acs.current().find(matchState);
  return itr != acs.current().end();
}

void NFA::stepThroughSplitStates() {
  acs.next().clear();
  for (auto& a : acs.current()) {
    checkForSplitState(a, a->matchPos, acs.next());
  }
  acs.update();
}

void NFA::checkForSplitState(NFA::State* st, size_t pos, Actives& ac) {
  if (st == nullptr) return;
  st->matchPos = pos;
  if (st->c != Specials::Split) {
    ac.insert(st);
  } else {
    checkForSplitState(st->next, pos, ac);
    checkForSplitState(st->other, pos, ac);
  }
}

void NFA::parseChar(char c, CompilerState& cState) {
  if (cState.isUnderSqBracket) {
    parseInsideSqBracket(c, cState);
    return;
  }
  if (cState.prevBackSlash) {
    cState.prevBackSlash = false;
    switch(c) {
    case 'd':
      addNewStateFor(Specials::Digit);
      break;
    case 's':
      addNewStateFor(Specials::WhiteSpace);
      break;
    case 'S':
      addNewStateFor(Specials::NonWhiteSpace);
      break;
    case '\\':
    case '[':
    case ']':
    case '(':
    case ')':
    case '.':
    case '+':
    case '*':
    case '?':
      addNewStateFor(c);
      break;
    default:
      ASSERT(false, "Bad escaped character \\%c!", c);
    };
    return;
  }
  parseGeneral(c, cState);
}

void NFA::parseGeneral(char c, CompilerState& cState) {
  switch(c) {
  case '+': {
    auto& frag = fragments.top();
    auto* sp = createState(Specials::Split);
    sp->other = frag.entry;
    frag.addState(sp);
  } break;
  case '*': {
    auto& frag = fragments.top();
    auto* sp = createState(Specials::Split);
    sp->other = frag.entry;
    frag.entry = sp;
    frag.addState(sp);
  } break;
  case '?': {
    auto& frag = fragments.top();
    auto* sp = createState(Specials::Split);
    sp->other = frag.entry;
    frag.entry = sp;
    frag.appendState(sp);
  } break;
  case '|':
    // This will be stitched properly during `stitchFragments()`
    addNewStateFor(Specials::Alternation);
    break;
  case '(':
    // This will be stitched properly during `stitchFragments()`
    addNewStateFor(Specials::Bracket);
    break;
  case ')':
    stitchFragments();
    break;
  case '[':
    cState.prevSqBracketOpen = true;
    cState.isUnderSqBracket = true;
    cState.isUnderRange = false;
    addNewStateFor(Specials::AnyFromList);
    break;
  case '.':
    addNewStateFor(Specials::Any);
    break;
  case '\\':
    cState.prevBackSlash = true;
    break;
  default:
    addNewStateFor(c);
    break;
  };
}

void NFA::parseInsideSqBracket(char c, CompilerState& cState) {
  auto prevSq = cState.prevSqBracketOpen;
  cState.prevSqBracketOpen = false;
  auto& frag = fragments.top();
  auto& str = frag.entry->s;
  // A-Z, 0-9, kind of ranges get higher priority than all the "special" chars
  // inside the [...]!
  if (cState.isUnderRange) {
    cState.isUnderRange = false;
    str.pop_back();   // this will be '-'
    auto start = str.back();
    if (start < c) {
      for (++start; start <= c; ++start) str.push_back(start);
    } else {
      for (--start; start >= c; --start) str.push_back(start);
    }
    return;
  }
  // to be considered a ']' literally, it must always come at the beginning or
  // in the case of ranges, it should immediately come after '-'
  if (c == ']') {
    if (prevSq) str.push_back(c);
    else cState.isUnderSqBracket = false;
    return;
  }
  if (c == '^') {
    if (prevSq) frag.entry->c = Specials::NoneFromList;
    else str.push_back(c);
    return;
  }
  // to be considered a '-' literally, it must always come at the beginning!
  if (c == '-' && !prevSq) cState.isUnderRange = true;
  str.push_back(c);
}

NFA::State* NFA::createState(int c) {
  auto* st = new State(c);
  states.push_back(st);
  return st;
}

void NFA::addNewStateFor(int c) {
  auto* st = createState(c);
  Fragment frag(st);
  fragments.push(frag);
}

void NFA::stitchFragments() {
  if (fragments.size() <= 1) return;
  auto frag = fragments.top();
  fragments.pop();
  if (frag.entry->c == Specials::Bracket) return;
  auto top = fragments.top();
  fragments.pop();
  if (top.entry->c == Specials::Bracket) {
    fragments.push(frag);
    return;
  }
  // alternation
  if (top.entry->c == Specials::Alternation) {
    top.entry->c = Specials::Split;
    ASSERT(!fragments.empty(),
           "Alternation must consist of atleast 2 fragments!");
    stitchFragments();
    auto& other = fragments.top();
    top.entry->next = frag.entry;
    top.tails = frag.tails;
    top.entry->other = other.entry;
    for (auto t : other.tails) top.appendState(t);
    fragments.pop();
    fragments.push(top);
    return;
  }
  // normal concatenation
  top.addState(frag.entry);
  top.tails = frag.tails;
  fragments.push(top);
  stitchFragments();
}

}  // namespace parser
}  // namespace teditor
