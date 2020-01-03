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

} // end namespace parser
} // end namespace teditor
