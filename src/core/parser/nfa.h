#pragma once

#include <stack>
#include <string>
#include <vector>
#include <unordered_set>
#include <core/double_buffer.hpp>
#include <core/pos2d.h>

namespace teditor {
namespace parser {

///@todo: add support for UTF-8 and/or unicode!


/**
 * @brief Ken-Thompson NFA as described here: https://swtch.com/~rsc/regexp/regexp1.html
 *        but adjusted to work with teditor environment
 * @note the current design is not thread-safe! Meaning, the same NFA object
 *       cannot be used by multiple threads at the same. It will cause
 *       corruption of data
 */
struct NFA {
  /**
   * @brief ctor with adding a regex for the NFA
   * @param reg regex
   */
  NFA(const std::string& reg);

  /**
   * @brief String match function
   * @param str the input string
   * @param start location from where to start searching
   * @param end location (minus 1) till where to search
   * @return the location of the longest match, else returns NFA::NoMatch
   */
  size_t find(const std::string& str, size_t start = 0, size_t end = 0);

  /**
   * @brief Tries for regex match starting from anywhere in the string
   * @param str the input string
   * @param matchStartPos will contain the starting location of the longest
   *                      match, if found, else, NFA::NoMatch
   * @param start location from where to start searching
   * @param end location (minus 1) till where to search
   * @return the location of the longest match, else returns NFA::NoMatch
   */
  size_t findAny(const std::string& str, size_t& matchStartPos,
                 size_t start = 0, size_t end = 0);

  ~NFA() { for (auto itr : states) delete itr; }

  /**
   * @defgroup ExplicitMethods To manually step during regex match phase
   * @{
   */
  /**
   * @brief checks if we have reached match state, indicating a regex match
   * @param lastStateRemaining if set to true, means that this will expect only
   *                           one state to be remaining which should be Match
   *                           state.
   * @return true if match has been found, else false
   */
  bool isMatch(bool lastStateRemaining = false) const;
  /**
   * @brief Tells if after the current `step()` whether there are any active
   *        states still remaining. So, this can be used to prematurely stop
   *        the regex search in the string
   * @return true if there are no more actives states, else false
   */
  bool areActiveStatesEmpty() const { return acs.current().empty(); }
  /**
   * @brief After the search has finished, use this to know the latest position
   *        of the match of the regex in the input string
   * @return if match found, its latest position in string, else NFA::NoMatch
   */
  const Point& getMatchPos() const { return matchState->matchPos; }
  /**
   * @brief Step through the NFA state using the current char
   * @param c current char
   * @param pos its position in the input string
   * @return true if input was consumed by any of the active states, else false
   */
  bool step(char c, const Point& pos);
  /**
   * @brief Reset all the variables used during regex search. This needs to be
   *        called once before beginning of every search
   */
  void reset();
  /** @} */

  /** represents case when regex didn't match anything */
  static const size_t NoMatch;

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
    Bracket,        // Temporary state for recognizing groups
    Alternation,    // Temporary state for recgonizing alternations
  };  // enum Specials

  ///@todo: special case for just string comparison (to reduce mem footprint)
  struct State {
    int c;
    std::string s;    // for matching with a set of possible chars (aka [...])
    State* next;
    State* other;     // used only with Split state
    Point matchPos;  // used only for matches
    State() : c(0), s(), next(nullptr), other(nullptr) {}
    State(int _c): c(_c), s(), next(nullptr), other(nullptr) {}
    bool isMatch(char in) const;
  };  // struct State

  typedef std::unordered_set<State*> Actives;

  std::string regex;
  std::vector<State*> states;  // all states for this NFA
  State *startState;
  State *matchState;
  // during matching these are the "active" states
  // this does NOT own the underlying pointers
  // uses double-buffering to avoid corruption
  DoubleBuffer<Actives> acs;

  void stepThroughSplitStates();
  void checkForSplitState(State* st, const Point& pos, Actives& ac);

  // used only while compiling the regex's
  // this does NOT own any of the underlying pointers
  struct Fragment {
    State* entry;
    std::vector<State*> tails;
    Fragment(): entry(nullptr), tails() {}
    Fragment(State* e);
    void addState(State* s);
    void appendState(State* s) { tails.push_back(s); }
  };  // struct Fragment

  // stack of fragments used during the regex compilation
  std::stack<Fragment> fragments;

  // used to store intermediate regex compiler state
  struct CompilerState {
    bool prevBackSlash;
    bool prevSqBracketOpen;
    bool isUnderRange;
    bool isUnderSqBracket;
    CompilerState();
    void validate(const std::string& reg);
  };  // struct CompilerState

  void parseChar(char c, CompilerState& cState);
  void parseGeneral(char c, CompilerState& cState);
  void parseInsideSqBracket(char c, CompilerState& cState);
  State* createState(int c);
  void addNewStateFor(int c);
  void stitchFragments();
};  // struct NFA

}  // namespace parser
}  // namespace teditor
