#include "core/parser/nfa.h"
#include "catch.hpp"
#include <string>

namespace teditor {
namespace parser {

TEST_CASE("NFA::general") {
  NFA nfa;
  SECTION("simple") {
    nfa.addRegex("a");
  }
  SECTION("+") {
    nfa.addRegex("ab+");
  }
  SECTION("*") {
    nfa.addRegex("ab*");
  }
  SECTION("?") {
    nfa.addRegex("ab?");
  }
  SECTION(".") {
    nfa.addRegex("ab.");
  }
  SECTION("|") {
    nfa.addRegex("a|b");
  }
}

TEST_CASE("NFA::BackSlash") {
  NFA nfa;
  SECTION("\\s") {
    nfa.addRegex("\\s");
  }
  SECTION("\\S") {
    nfa.addRegex("\\S");
  }
  SECTION("\\d") {
    nfa.addRegex("\\d");
  }
  SECTION("\\Z") {  // has no meaning in the current NFA logic!
    nfa.addRegex("\\Z");
  }
}

TEST_CASE("NFA::SqBrkt") {
  NFA nfa;
  SECTION("any-from-list") {
    nfa.addRegex("[abcd]");
  }
  SECTION("range") {
    nfa.addRegex("[a-d]");
  }
  SECTION("range-with-minus") {
    nfa.addRegex("[0--]");  // -, ., / and 0
  }
  SECTION("reverse-range") {
    nfa.addRegex("[d-a]");
  }
  SECTION("only-minus") {
    nfa.addRegex("[-]");
  }
  SECTION("sq-brkt-in-range") {
    nfa.addRegex("[[-]]");  // [, \ and ]
  }
  SECTION("literal-^") {
    nfa.addRegex("[ab^]");  // a, b, and ^
  }
  SECTION("^ aka none-from-list") {
    nfa.addRegex("[^ab]");
  }
}

} // end namespace parser
} // end namespace teditor
