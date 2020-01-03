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
  //printf("start...\n");
  if (end == 0) end = str.size();
  // prepare the engine for the current match task
  acs.current().clear();
  for (auto s : startStates) acs.current().insert(s);
  //printf("actives count = %lu start states = %lu\n", acs.current().size(), startStates.size());
  for (; start < end; ++start) {
    stepThroughSplitStates();
    acs.next().clear();
    //printf("current char = %c(%d) pos = %lu\n", str[start], str[start], start);
    for (auto& a : acs.current()) {
      auto& next = acs.next();
      // //printf("state-char = %d char = %d start = %lu pos = %lu\n",
      //        a->c, str[start], start, a->matchPos);
      if (a->c == Specials::Match) {
        next.insert(a);
        continue;
      }
      //printf("current state = %d\n", a->c);
      if (a->isMatch(str[start])) {
        if (a->next != nullptr) {
          //printf("  inserting next c=%d\n", a->next->c);
          a->next->matchPos = start;
          next.insert(a->next);
        }
        if (a->other != nullptr) {
          //printf("  inserting other c=%d\n", a->other->c);
          a->other->matchPos = start;
          next.insert(a->other);
        }
      }
    }
    acs.update();
    if (acs.current().empty()) return NoMatch;
  }
  stepThroughSplitStates();
  // always finds the longest match!
  size_t pos = 0;
  bool matchFound = false;
  for (auto& a : acs.current()) {
    //printf("final state c=%d\n", a->c);
    if (a->c == Match) {
      //printf("match found... pos=%lu\n", a->matchPos);
      matchFound = true;
      pos = std::max(a->matchPos, pos);
    }
  }
  //printf("end...\n");
  return matchFound ? pos : NoMatch;
}

void NFA::addRegex(const std::string& reg) {
  CompilerState cState;
  for (auto c : reg) parseChar(c, cState);
  cState.validate(reg);
  //printf("stitch start len=%lu...\n", fragments.size());
  stitchFragments();
  // reached the end of the list, note down its start state and add match state
  ASSERT(fragments.size() == 1,
         "After stitching, there should only be one fragment left! [%lu]",
         fragments.size());
  auto& frag = fragments.top();
  //printf("last fragment entry = %d\n", frag.entry->c);
  startStates.push_back(frag.entry);
  auto* st = createState(Specials::Match);
  frag.addState(st);
  fragments.pop();
  //printf("stitch end...\n");
}

void NFA::stepThroughSplitStates() {
  acs.next().clear();
  for (auto& a : acs.current()) {
    //printf("working on state = %d\n", a->c);
    checkForSplitState(a, a->matchPos, acs.next());
  }
  acs.update();
}

void NFA::checkForSplitState(NFA::State* st, size_t pos, Actives& ac) {
  if (st == nullptr) return;
  //printf("checking for split-state jump for st->c=%d .. #states=%lu\n", st->c, states.size());
  st->matchPos = pos;
  if (st->c != Specials::Split) {
    ac.insert(st);
  } else {
    checkForSplitState(st->next, pos, ac);
    checkForSplitState(st->other, pos, ac);
  }
}

void NFA::parseChar(char c, CompilerState& cState) {
  //printf("prevBS = %d char = %c\n", cState.prevBackSlash, c);
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
  //printf("current char for regex compiling = %c\n", c);
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
    //printf("inside backslash handler\n");
    //printf("  prevBS = %d char = %c\n", cState.prevBackSlash, c);
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
  //printf("creating state for %d\n", c);
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
  auto top = fragments.top();
  fragments.pop();
  //printf("stitching: frag.entry=%d top.entry=%d\n", frag.entry->c, top.entry->c);
  // alternation
  if (top.isOnlySplit()) {
    ASSERT(!fragments.empty(),
           "Alternation must consist of atleast 2 fragments!");
    stitchFragments();
    auto& other = fragments.top();
    //printf("   other.entry (for alternation)=%d\n", other.entry->c);
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
