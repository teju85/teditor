#include "core/parser/nfa.h"
#include "catch.hpp"
#include <string>

namespace teditor {
namespace parser {

TEST_CASE("NFA::general") {
  SECTION("simple1") {
    NFA nfa("a");
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("b") == NFA::NoMatch);
    REQUIRE(nfa.find("hello", 2) == NFA::NoMatch);
    REQUIRE(nfa.find("hallo", 1) == 1);
  }
  SECTION("simple2") {
    NFA nfa("abc");
    REQUIRE(nfa.find("acb") == NFA::NoMatch);
    REQUIRE(nfa.find("b") == NFA::NoMatch);
    REQUIRE(nfa.find("hello", 2) == NFA::NoMatch);
    REQUIRE(nfa.find(" abc", 1) == 3);
  }
  SECTION("+") {
    NFA nfa("ab+");
    REQUIRE(nfa.find("a") == NFA::NoMatch);
    REQUIRE(nfa.find("aab") == NFA::NoMatch);
    REQUIRE(nfa.find("ab") == 1);
    REQUIRE(nfa.find("abb") == 2);
    REQUIRE(nfa.find("aab", 1) == 2);
  }
  SECTION("*") {
    NFA nfa("ab*");
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("cab") == NFA::NoMatch);
    REQUIRE(nfa.find("acc") == 0);
    REQUIRE(nfa.find("ab") == 1);
    REQUIRE(nfa.find("abb") == 2);
    REQUIRE(nfa.find("abbbbbc") == 5);
  }
  SECTION("?") {
    NFA nfa("ab?");
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("ab") == 1);
    REQUIRE(nfa.find("cab") == NFA::NoMatch);
    REQUIRE(nfa.find("abb") == 1);
  }
  SECTION(".") {
    NFA nfa("ab.");
    REQUIRE(nfa.find("a") == NFA::NoMatch);
    REQUIRE(nfa.find("ab") == NFA::NoMatch);
    REQUIRE(nfa.find("abx") == 2);
    REQUIRE(nfa.find("aby") == 2);
    REQUIRE(nfa.find("axx") == NFA::NoMatch);
  }
  SECTION("|") {
    NFA nfa("a|b");
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("b") == 0);
    REQUIRE(nfa.find("acc") == 0);
    REQUIRE(nfa.find("bcc") == 0);
    REQUIRE(nfa.find("c") == NFA::NoMatch);
  }
  SECTION("| - case 1") {
    NFA nfa("a|b|c");
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("b") == 0);
    REQUIRE(nfa.find("c") == 0);
    REQUIRE(nfa.find("d") == NFA::NoMatch);
  }
  SECTION("| - case 2") {
    NFA nfa("abc|def");
    REQUIRE(nfa.find("def") == 2);
    REQUIRE(nfa.find("abc") == 2);
    REQUIRE(nfa.find("ghi") == NFA::NoMatch);
  }
}

TEST_CASE("NFA::BackSlash") {
  SECTION("\\s") {
    NFA nfa("\\s");
    REQUIRE(nfa.find("abcdef") == NFA::NoMatch);
    REQUIRE(nfa.find("Hello World") == NFA::NoMatch);
    REQUIRE(nfa.find("Hello World", 5) == 5);
  }
  SECTION("\\S") {
    NFA nfa("\\S");
    REQUIRE(nfa.find("abcd") == 0);
    REQUIRE(nfa.find(" abcd") == NFA::NoMatch);
    REQUIRE(nfa.find("\tabcd") == NFA::NoMatch);
  }
  SECTION("\\d") {
    NFA nfa("\\d");
    REQUIRE(nfa.find("0abcs") == 0);
    REQUIRE(nfa.find("abcs") == NFA::NoMatch);
  }
  SECTION("\\\\") {
    NFA nfa("\\\\");
    REQUIRE(nfa.find("\\") == 0);
    REQUIRE(nfa.find("a\\") == NFA::NoMatch);
  }
  SECTION("\\[") {
    NFA nfa("\\[");
    REQUIRE(nfa.find("[") == 0);
    REQUIRE(nfa.find("a[") == NFA::NoMatch);
  }
  SECTION("\\]") {
    NFA nfa("\\]");
    REQUIRE(nfa.find("]") == 0);
    REQUIRE(nfa.find("a]") == NFA::NoMatch);
  }
  SECTION("\\(") {
    NFA nfa("\\(");
    REQUIRE(nfa.find("(") == 0);
    REQUIRE(nfa.find("a(") == NFA::NoMatch);
  }
  SECTION("\\)") {
    NFA nfa("\\)");
    REQUIRE(nfa.find(")") == 0);
    REQUIRE(nfa.find("a)") == NFA::NoMatch);
  }
  SECTION("\\.") {
    NFA nfa("\\.");
    REQUIRE(nfa.find(".") == 0);
    REQUIRE(nfa.find("a.") == NFA::NoMatch);
  }
  SECTION("\\+") {
    NFA nfa("\\+");
    REQUIRE(nfa.find("+") == 0);
    REQUIRE(nfa.find("a+") == NFA::NoMatch);
  }
  SECTION("\\*") {
    NFA nfa("\\*");
    REQUIRE(nfa.find("*") == 0);
    REQUIRE(nfa.find("a*") == NFA::NoMatch);
  }
  SECTION("\\?") {
    NFA nfa("\\?");
    REQUIRE(nfa.find("?") == 0);
    REQUIRE(nfa.find("a?") == NFA::NoMatch);
  }
  SECTION("\\Z") {
    auto lambda = []() { NFA nfa("\\Z"); };
    REQUIRE_THROWS(lambda());
  }
  SECTION("\\d+") {
    NFA nfa("\\d+");
    REQUIRE(nfa.find("01234") == 4);
    REQUIRE(nfa.find("abcs") == NFA::NoMatch);
  }
}

TEST_CASE("NFA::SqBrkt") {
  SECTION("any-from-list") {
    NFA nfa("[abcd]");
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("b") == 0);
    REQUIRE(nfa.find("c") == 0);
    REQUIRE(nfa.find("d") == 0);
    REQUIRE(nfa.find("e") == NFA::NoMatch);
  }
  SECTION("range") {
    NFA nfa("[a-d]");
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("b") == 0);
    REQUIRE(nfa.find("c") == 0);
    REQUIRE(nfa.find("d") == 0);
    REQUIRE(nfa.find("e") == NFA::NoMatch);
  }
  SECTION("range-with-minus") {
    NFA nfa("[0--]");  // -, ., / and 0
    REQUIRE(nfa.find("0") == 0);
    REQUIRE(nfa.find("/") == 0);
    REQUIRE(nfa.find(".") == 0);
    REQUIRE(nfa.find("-") == 0);
    REQUIRE(nfa.find("a") == NFA::NoMatch);
  }
  SECTION("reverse-range") {
    NFA nfa("[d-a]");
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("b") == 0);
    REQUIRE(nfa.find("c") == 0);
    REQUIRE(nfa.find("d") == 0);
    REQUIRE(nfa.find("e") == NFA::NoMatch);
  }
  SECTION("only-minus") {
    NFA nfa("[-]");
    REQUIRE(nfa.find("-") == 0);
    REQUIRE(nfa.find("a") == NFA::NoMatch);
  }
  SECTION("sq-brkt-in-range") {
    NFA nfa("[[-]]");  // [, \ and ]
    REQUIRE(nfa.find("[") == 0);
    REQUIRE(nfa.find("\\") == 0);
    REQUIRE(nfa.find("]") == 0);
    REQUIRE(nfa.find("e") == NFA::NoMatch);
  }
  SECTION("literal-^") {
    NFA nfa("[ab^]");  // a, b, and ^
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("b") == 0);
    REQUIRE(nfa.find("^") == 0);
    REQUIRE(nfa.find("e") == NFA::NoMatch);
  }
  SECTION("^ aka none-from-list") {
    NFA nfa("[^ab]");
    REQUIRE(nfa.find("a") == NFA::NoMatch);
    REQUIRE(nfa.find("b") == NFA::NoMatch);
    REQUIRE(nfa.find("c") == 0);
  }
  SECTION("multiple-ranges") {
    NFA nfa("[a-ce-g]");
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("b") == 0);
    REQUIRE(nfa.find("c") == 0);
    REQUIRE(nfa.find("e") == 0);
    REQUIRE(nfa.find("f") == 0);
    REQUIRE(nfa.find("g") == 0);
    REQUIRE(nfa.find("d") == NFA::NoMatch);
  }
}

TEST_CASE("NFA::Groups") {
  SECTION("simple1") {
    NFA nfa("(abc)");
    REQUIRE(nfa.find("abc") == 2);
    REQUIRE(nfa.find("aabc") == NFA::NoMatch);
  }
  SECTION("simple2") {
    NFA nfa("(abc)+");
    REQUIRE(nfa.find("abcabc") == 5);
    REQUIRE(nfa.find("aba") == NFA::NoMatch);
  }
  SECTION("simple3") {
    NFA nfa("abcd+");
    REQUIRE(nfa.find("abcd") == 3);
    REQUIRE(nfa.find("abcdd") == 4);
    REQUIRE(nfa.find("abc") == NFA::NoMatch);
  }
  SECTION("simple4") {
    NFA nfa("(abcd)+");
    REQUIRE(nfa.find("abcd") == 3);
    REQUIRE(nfa.find("abcdabcd") == 7);
    REQUIRE(nfa.find("abc") == NFA::NoMatch);
  }
  SECTION("simple5") {
    NFA nfa("ab|(cd)+");
    REQUIRE(nfa.find("ab") == 1);
    REQUIRE(nfa.find("cd") == 1);
    REQUIRE(nfa.find("cdcd") == 3);
    REQUIRE(nfa.find("ac") == NFA::NoMatch);
    REQUIRE(nfa.find("ca") == NFA::NoMatch);
  }
}

#define FIND_ANY(nfa, str, refStart, refEnd)    do {                    \
    size_t startPos;                                                    \
    auto pos = nfa.findAny(str, startPos);                              \
    REQUIRE(pos == refEnd);                                             \
    REQUIRE(startPos == refStart);                                      \
  } while(0)

TEST_CASE("NFA::findany") {
  SECTION("simple1") {
    NFA nfa("a");
    FIND_ANY(nfa, "a", 0, 0);
    FIND_ANY(nfa, "b", NFA::NoMatch, NFA::NoMatch);
    FIND_ANY(nfa, "hello", NFA::NoMatch, NFA::NoMatch);
    FIND_ANY(nfa, "hallo", 1, 1);
  }
  SECTION("simple2") {
    NFA nfa("abc");
    FIND_ANY(nfa, "acb", NFA::NoMatch, NFA::NoMatch);
    FIND_ANY(nfa, "hello-abc", 6, 8);
  }
}

#undef FIND_ANY

} // end namespace parser
} // end namespace teditor
