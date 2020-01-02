#pragma once

#include <stack>
#include <string>
#include <vector>
#include <unordered_set>

namespace teditor {
namespace parser {

///@todo: add support for UTF-8 and/or unicode!


/**
 * @brief A single NFA used to match a list of tokens (as seen during lexing)
 */
struct NFA {
  /**
   * @brief Compile the input regex and add it as one of the alternations
   * 
   * The idea here is that this NFA represents alternations of *all* of the
   * regex's which describe lex-tokens in the grammar. This will make the lex
   * logic far efficient as it does not require back-tracking
   *
   * @param reg regex that defines the current token
   */
  void addRegex(const std::string& reg);

  /** after the search is done, this will tell if there was a match or not */
  bool isMatch() const {
    for (auto s : actives) if (s->c == Match) return true;
    return false;
  }

private:
  /** list of special states */
  enum Specials {
    Split = 256,    // splitter NFA state
    Match,          // terminal state
    Digit,          // \d
    WhiteSpace,     // \s
    NonWhiteSpace,  // \S
    Any,            // .
    AnyFromList,    // [...], [A-Z], [a-z], [0-9]
    NoneFromList,   // [^...]
  };  // enum Specials


  ///@todo: special case for just string comparison (to reduce mem footprint)
  struct State {
    int c;
    std::string s;  // when matching with a set of possible chars (aka [...]) */
    State* next;
    State* other;  // used only with Split state

    State() : c(0), s(), next(nullptr), other(nullptr) {}
    State(int _c): c(_c), s(), next(nullptr), other(nullptr) {}
    bool isMatch(char in) const;
  };  // struct State


  struct States : public std::vector<State*> {
    ~States() { for (auto itr : *this) delete itr; }
  };  // struct States


  // used only while compiling the regex's
  // this does NOT own any of the underlying pointers
  struct Fragment {
    State* entry;
    States tails;

    Fragment(): entry(nullptr), tails() {}
    Fragment(State* e);
    void addState(State* s);
    void appendState(State* s) { tails.push_back(s); }
  };  // struct Fragment


  // used to store intermediate regex compiler state
  struct CompilerState {
    bool prevBackSlash;
    bool prevSqBracketOpen;
    bool isUnderRange;
    bool isUnderSqBracket;

    CompilerState();
  };  // struct CompilerState


  States states;  // all states for this NFA
  // one start state pointer for each lexer-token regex
  // this does NOT own the underlying pointers
  std::vector<State*> startStates;
  // during matching these are the "active" states
  // this does NOT own the underlying pointers
  std::unordered_set<State*> actives;
  // stack of fragments used during the regex compilation
  std::stack<Fragment> fragments;

  // prepares the engine for the next match task
  void reset() {
    actives.clear();
    for (auto s : startStates) actives.insert(s);
  }

  void parseChar(char c, CompilerState& cState);
  void parseGeneral(char c, CompilerState cState);
  void parseInsideSqBracket(char c, CompilerState& cState);
  State* createState(int c);
  void addNewStateFor(int c);
  void stitchFragments();
};  // struct NFA

}  // namespace parser
}  // namespace teditor
