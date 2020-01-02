#include "nfa.h"
#include <core/utils.h>

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

void NFA::addRegex(const std::string& reg) {
  CompilerState cState;
  for (auto c : reg) parseChar(c, cState);
  cState.validate(reg);
  stitchFragments();
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
  if (c == '-' && !prevSq) {
    cState.isUnderRange = true;
    str.push_back(c);
    return;
  }
}

NFA::State* NFA::createState(int c) {
  auto* st = new State(c);
  states.push_back(st);
  return st;
}

void NFA::addNewStateFor(int c) {
  auto* st = createState(c);
  if (fragments.empty()) {
    Fragment frag(st);
    fragments.push(frag);
  } else {
    auto& frag = fragments.top();
    frag.addState(st);
  }
}

void NFA::stitchFragments() {
}

}  // namespace parser
}  // namespace teditor
