#include "nfa.h"
#include <core/utils.h>

namespace teditor {
namespace parser {

const size_t NFA::NoMatch = std::string::npos;

bool NFA::State::isMatch(char in) const {
  switch(c) {
  case Split:
  case Match:         return true;
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

bool NFA::Fragment::isOnlySplit() const {
  return entry != nullptr && entry->c == Specials::Split && !tails.empty() &&
    tails[0] == entry;
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

size_t NFA::find(const std::string& str, size_t start, size_t end) {
  if (end == 0) end = str.size();
  // prepare the engine for the current match task
  acs.current().clear();
  for (auto s : startStates) acs.current()[s] = NoMatch;
  for (; start < end; ++start) {
    stepThroughSplitStates();
    acs.next().clear();
    for (auto& a : acs.current()) {
      auto& next = acs.next();
      //printf("state-char = %d char = %d start = %lu\n", a.first->c, str[start], start);
      if (a.first->c == Specials::Match) {
        next[a.first] = a.second;
        continue;
      }
      if (a.first->isMatch(str[start])) {
        if (a.first->next != nullptr) next[a.first->next] = start;
        if (a.first->other != nullptr) next[a.first->other] = start;
      }
    }
    acs.update();
    if (acs.current().empty()) return NoMatch;
  }
  // always finds the longest match!
  size_t pos = 0;
  bool matchFound = false;
  for (auto& a : acs.current()) {
    if (a.first->c == Match) {
      matchFound = true;
      pos = std::max(a.second, pos);
    }
  }
  return matchFound ? pos : NoMatch;
}

void NFA::addRegex(const std::string& reg) {
  CompilerState cState;
  for (auto c : reg) parseChar(c, cState);
  cState.validate(reg);
  stitchFragments();
}

void NFA::stepThroughSplitStates() {
  acs.next().clear();
  for (auto& a : acs.current())
    checkForSplitState(a.first, a.second, acs.next());
  acs.update();
}

void NFA::checkForSplitState(NFA::State* st, size_t pos, Actives& ac) {
  if (st == nullptr) return;
  if (st->c != Specials::Split) {
    ac[st] = pos;
    return;
  }
  checkForSplitState(st->next, pos, ac);
  checkForSplitState(st->other, pos, ac);
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
    default:
      addNewStateFor(c);
    };
    return;
  }
  parseGeneral(c, cState);
}

void NFA::parseGeneral(char c, CompilerState cState) {
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
    frag.addState(sp);
  } break;
  case '?': {
    auto& frag = fragments.top();
    auto* sp = createState(Specials::Split);
    sp->other = frag.entry;
    frag.entry = sp;
    frag.appendState(sp);
  } break;
  case '|': {
    // just create a fragment containing split state. This will be stitched
    // properly during `stitchFragments` call
    auto* sp = createState(Specials::Split);
    Fragment frag(sp);
    fragments.push(frag);
  } break;
  case '(':
    ///@todo: implement
    break;
  case ')':
    ///@todo: implement
    break;
  case '[': {
    cState.prevSqBracketOpen = true;
    cState.isUnderSqBracket = true;
    cState.isUnderRange = false;
    auto* st = createState(Specials::AnyFromList);
    Fragment frag(st);
    fragments.push(frag);
  } break;
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
  if (fragments.empty()) return;
  auto frag = fragments.top();
  fragments.pop();
  // reached the end of the list, note down its start state and add match state
  if (fragments.empty()) {
    startStates.push_back(frag.entry);
    auto* st = createState(Specials::Match);
    frag.addState(st);
    return;
  }
  auto top = fragments.top();
  fragments.pop();
  if (top.isOnlySplit()) {
    ASSERT(!fragments.empty(),
           "Alternation must consist of atleast 2 fragments!");
    auto& other = fragments.top();
    top.entry->next = frag.entry;
    top.tails = frag.tails;
    top.entry->other = other.entry;
    for (auto t : other.tails) top.appendState(t);
    fragments.pop();
    fragments.push(top);
    return;
  }
  top.addState(frag.entry);
  top.tails = frag.tails;
  fragments.push(top);
  stitchFragments();
}

}  // namespace parser
}  // namespace teditor
