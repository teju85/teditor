#include "core/parser/nfa.h"
#include "catch.hpp"
#include <string>

namespace teditor {
namespace parser {

TEST_CASE("NFA") {
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
}

} // end namespace parser
} // end namespace teditor
