#include "nfa.h"

namespace teditor {
namespace parser {

bool NFA::State::isMatch(char in) const {
  switch(c) {
  case Split:
  case Match:         return true;
  case WhiteSpace:    return in == ' ' || in == '\t';
  case NonWhiteSpace: return in != ' ' && in != '\t';
  case Any:           return true;
  case AnyFromList:   return s.find(in) != std::string::npos;
  case NoneFromList:  return s.find(in) == std::string::npos;
  default:            return (c & 0xff) == in;
  };
}

NFA::Fragment::Fragment(State* e): entry(e), tails() {
  tails.push_back(e);
}

void NFA::Fragment::addState(State* s) {
  for (auto t : tails) t->next = s;
  tails.clear();
  tails.push_back(s);
}


NFA::CompilerState::CompilerState() :
  prevBackSlash(false), prevSqBracketOpen(false), isUnderRange(false),
  isUnderSqBracket(false) {
}

void NFA::addRegex(const std::string& reg) {
  while (!fragments.empty()) fragments.pop();
  CompilerState cState;
  for (auto c : reg) parseChar(c, cState);
}

void NFA::parseChar(char c, CompilerState& cState) {
  if (cState.isUnderSqBracket) {
    parseInsideSqBracket(c, cState);
    return;
  }
  if (cState.prevBackSlash) {
    cState.prevBackSlash = false;
    switch(c) {
    case 'd': {
      auto* st = new State;
      st->c = Specials::Digit;
      addNewState(st);
    } break;
    case 's': {
      auto* st = new State;
      st->c = Specials::WhiteSpace;
      addNewState(st);
    } break;
    case 'S': {
      auto* st = new State;
      st->c = Specials::NonWhiteSpace;
      addNewState(st);
    } break;
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
    auto* sp = new State;
    sp->c = Specials::Split;
    sp->other = frag.entry;
    frag.addState(sp);
  } break;
  case '*': {
    auto& frag = fragments.top();
    auto* sp = new State;
    sp->c = Specials::Split;
    sp->other = frag.entry;
    frag.addState(sp);
  } break;
  case '?': {
    auto& frag = fragments.top();
    auto* sp = new State;
    sp->c = Specials::Split;
    sp->other = frag.entry;
    frag.entry = sp;
    frag.tails.push_back(sp);
  } break;
  case '|':
    ///@todo: implement
    break;
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
    auto* st = new State;
    st->c = Specials::AnyFromList;
    states.push_back(st);
    Fragment frag(st);
    fragments.push(frag);
  } break;
  case '.': {
    auto* st = new State;
    st->c = Specials::Any;
    addNewState(st);
  } break;
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
  if (c == '-' && !prevSq) {
    cState.isUnderRange = true;
    str.push_back(c);
    return;
  }
}

void NFA::addNewStateFor(char c) {
  auto* st = new State;
  st->c = c;
  addNewState(st);
}

void NFA::addNewState(State* st) {
  states.push_back(st);
  if (fragments.empty()) {
    Fragment frag(st);
    fragments.push(frag);
  } else {
    auto& frag = fragments.top();
    frag.addState(st);
  }
}

}  // namespace parser
}  // namespace teditor
