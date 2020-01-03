#include "core/parser/nfa.h"
#include "catch.hpp"
#include <string>

namespace teditor {
namespace parser {

TEST_CASE("NFA::general") {
  NFA nfa;
  SECTION("simple1") {
    nfa.addRegex("a");
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("b") == NFA::NoMatch);
    REQUIRE(nfa.find("hello", 2) == NFA::NoMatch);
    REQUIRE(nfa.find("hallo", 1) == 1);
  }
  SECTION("simple2") {
    nfa.addRegex("abc");
    REQUIRE(nfa.find("acb") == NFA::NoMatch);
    REQUIRE(nfa.find("b") == NFA::NoMatch);
    REQUIRE(nfa.find("hello", 2) == NFA::NoMatch);
    REQUIRE(nfa.find(" abc", 1) == 3);
  }
  SECTION("+") {
    nfa.addRegex("ab+");
    REQUIRE(nfa.find("a") == NFA::NoMatch);
    REQUIRE(nfa.find("aab") == NFA::NoMatch);
    REQUIRE(nfa.find("ab") == 1);
    REQUIRE(nfa.find("abb") == 2);
    REQUIRE(nfa.find("aab", 1) == 2);
  }
  SECTION("*") {
    nfa.addRegex("ab*");
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("cab") == NFA::NoMatch);
    REQUIRE(nfa.find("acc") == 0);
    REQUIRE(nfa.find("ab") == 1);
    REQUIRE(nfa.find("abb") == 2);
    REQUIRE(nfa.find("abbbbbc") == 5);
  }
  SECTION("?") {
    nfa.addRegex("ab?");
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("ab") == 1);
    REQUIRE(nfa.find("cab") == NFA::NoMatch);
    REQUIRE(nfa.find("abb") == 1);
  }
  SECTION(".") {
    nfa.addRegex("ab.");
    REQUIRE(nfa.find("a") == NFA::NoMatch);
    REQUIRE(nfa.find("ab") == NFA::NoMatch);
    REQUIRE(nfa.find("abx") == 2);
    REQUIRE(nfa.find("aby") == 2);
    REQUIRE(nfa.find("axx") == NFA::NoMatch);
  }
  SECTION("|") {
    nfa.addRegex("a|b");
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("b") == 0);
    REQUIRE(nfa.find("acc") == 0);
    REQUIRE(nfa.find("bcc") == 0);
    REQUIRE(nfa.find("c") == NFA::NoMatch);
  }
}

TEST_CASE("NFA::BackSlash") {
  NFA nfa;
  SECTION("\\s") {
    nfa.addRegex("\\s");
    REQUIRE(nfa.find("abcdef") == NFA::NoMatch);
    REQUIRE(nfa.find("Hello World") == NFA::NoMatch);
    REQUIRE(nfa.find("Hello World", 5) == 5);
  }
  SECTION("\\S") {
    nfa.addRegex("\\S");
    REQUIRE(nfa.find("abcd") == 0);
    REQUIRE(nfa.find(" abcd") == NFA::NoMatch);
    REQUIRE(nfa.find("\tabcd") == NFA::NoMatch);
  }
  SECTION("\\d") {
    nfa.addRegex("\\d");
    REQUIRE(nfa.find("0abcs") == 0);
    REQUIRE(nfa.find("abcs") == NFA::NoMatch);
  }
  SECTION("\\Z") {  // has no meaning in the current NFA logic!
    nfa.addRegex("\\Z");
    REQUIRE(nfa.find("Z") == 0);
    REQUIRE(nfa.find("aZ") == NFA::NoMatch);
  }
  SECTION("\\d+") {
    nfa.addRegex("\\d+");
    REQUIRE(nfa.find("01234") == 4);
    REQUIRE(nfa.find("abcs") == NFA::NoMatch);
  }
}

TEST_CASE("NFA::SqBrkt") {
  NFA nfa;
  SECTION("any-from-list") {
    nfa.addRegex("[abcd]");
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("b") == 0);
    REQUIRE(nfa.find("c") == 0);
    REQUIRE(nfa.find("d") == 0);
    REQUIRE(nfa.find("e") == NFA::NoMatch);
  }
  SECTION("range") {
    nfa.addRegex("[a-d]");
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("b") == 0);
    REQUIRE(nfa.find("c") == 0);
    REQUIRE(nfa.find("d") == 0);
    REQUIRE(nfa.find("e") == NFA::NoMatch);
  }
  // SECTION("range-with-minus") {
  //   nfa.addRegex("[0--]");  // -, ., / and 0
  // }
  // SECTION("reverse-range") {
  //   nfa.addRegex("[d-a]");
  // }
  // SECTION("only-minus") {
  //   nfa.addRegex("[-]");
  // }
  // SECTION("sq-brkt-in-range") {
  //   nfa.addRegex("[[-]]");  // [, \ and ]
  // }
  // SECTION("literal-^") {
  //   nfa.addRegex("[ab^]");  // a, b, and ^
  // }
  // SECTION("^ aka none-from-list") {
  //   nfa.addRegex("[^ab]");
  // }
}

} // end namespace parser
} // end namespace teditor
