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
  SECTION("| - case 1") {
    nfa.addRegex("a|b|c");
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("b") == 0);
    REQUIRE(nfa.find("c") == 0);
    REQUIRE(nfa.find("d") == NFA::NoMatch);
  }
  SECTION("| - case 2") {
    nfa.addRegex("abc|def");
    REQUIRE(nfa.find("def") == 2);
    REQUIRE(nfa.find("abc") == 2);
    REQUIRE(nfa.find("ghi") == NFA::NoMatch);
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
  SECTION("\\\\") {
    nfa.addRegex("\\\\");
    REQUIRE(nfa.find("\\") == 0);
    REQUIRE(nfa.find("a\\") == NFA::NoMatch);
  }
  SECTION("\\[") {
    nfa.addRegex("\\[");
    REQUIRE(nfa.find("[") == 0);
    REQUIRE(nfa.find("a[") == NFA::NoMatch);
  }
  SECTION("\\]") {
    nfa.addRegex("\\]");
    REQUIRE(nfa.find("]") == 0);
    REQUIRE(nfa.find("a]") == NFA::NoMatch);
  }
  SECTION("\\(") {
    nfa.addRegex("\\(");
    REQUIRE(nfa.find("(") == 0);
    REQUIRE(nfa.find("a(") == NFA::NoMatch);
  }
  SECTION("\\)") {
    nfa.addRegex("\\)");
    REQUIRE(nfa.find(")") == 0);
    REQUIRE(nfa.find("a)") == NFA::NoMatch);
  }
  SECTION("\\.") {
    nfa.addRegex("\\.");
    REQUIRE(nfa.find(".") == 0);
    REQUIRE(nfa.find("a.") == NFA::NoMatch);
  }
  SECTION("\\+") {
    nfa.addRegex("\\+");
    REQUIRE(nfa.find("+") == 0);
    REQUIRE(nfa.find("a+") == NFA::NoMatch);
  }
  SECTION("\\*") {
    nfa.addRegex("\\*");
    REQUIRE(nfa.find("*") == 0);
    REQUIRE(nfa.find("a*") == NFA::NoMatch);
  }
  SECTION("\\?") {
    nfa.addRegex("\\?");
    REQUIRE(nfa.find("?") == 0);
    REQUIRE(nfa.find("a?") == NFA::NoMatch);
  }
  SECTION("\\Z") {
    REQUIRE_THROWS(nfa.addRegex("\\Z"));
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
  SECTION("range-with-minus") {
    nfa.addRegex("[0--]");  // -, ., / and 0
    REQUIRE(nfa.find("0") == 0);
    REQUIRE(nfa.find("/") == 0);
    REQUIRE(nfa.find(".") == 0);
    REQUIRE(nfa.find("-") == 0);
    REQUIRE(nfa.find("a") == NFA::NoMatch);
  }
  SECTION("reverse-range") {
    nfa.addRegex("[d-a]");
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("b") == 0);
    REQUIRE(nfa.find("c") == 0);
    REQUIRE(nfa.find("d") == 0);
    REQUIRE(nfa.find("e") == NFA::NoMatch);
  }
  SECTION("only-minus") {
    nfa.addRegex("[-]");
    REQUIRE(nfa.find("-") == 0);
    REQUIRE(nfa.find("a") == NFA::NoMatch);
  }
  SECTION("sq-brkt-in-range") {
    nfa.addRegex("[[-]]");  // [, \ and ]
    REQUIRE(nfa.find("[") == 0);
    REQUIRE(nfa.find("\\") == 0);
    REQUIRE(nfa.find("]") == 0);
    REQUIRE(nfa.find("e") == NFA::NoMatch);
  }
  SECTION("literal-^") {
    nfa.addRegex("[ab^]");  // a, b, and ^
    REQUIRE(nfa.find("a") == 0);
    REQUIRE(nfa.find("b") == 0);
    REQUIRE(nfa.find("^") == 0);
    REQUIRE(nfa.find("e") == NFA::NoMatch);
  }
  SECTION("^ aka none-from-list") {
    nfa.addRegex("[^ab]");
    REQUIRE(nfa.find("a") == NFA::NoMatch);
    REQUIRE(nfa.find("b") == NFA::NoMatch);
    REQUIRE(nfa.find("c") == 0);
  }
  SECTION("multiple-ranges") {
    nfa.addRegex("[a-ce-g]");
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
  NFA nfa;
  SECTION("simple1") {
    nfa.addRegex("(abc)");
    REQUIRE(nfa.find("abc") == 2);
    REQUIRE(nfa.find("aabc") == NFA::NoMatch);
  }
  SECTION("simple2") {
    nfa.addRegex("(abc)+");
    REQUIRE(nfa.find("abcabc") == 5);
    REQUIRE(nfa.find("aba") == NFA::NoMatch);
  }
  SECTION("simple3") {
    nfa.addRegex("abcd+");
    REQUIRE(nfa.find("abcd") == 3);
    REQUIRE(nfa.find("abcdd") == 4);
    REQUIRE(nfa.find("abc") == NFA::NoMatch);
  }
  SECTION("simple4") {
    nfa.addRegex("(abcd)+");
    REQUIRE(nfa.find("abcd") == 3);
    REQUIRE(nfa.find("abcdabcd") == 7);
    REQUIRE(nfa.find("abc") == NFA::NoMatch);
  }
  SECTION("simple5") {
    nfa.addRegex("ab|(cd)+");
    REQUIRE(nfa.find("ab") == 1);
    REQUIRE(nfa.find("cd") == 1);
    REQUIRE(nfa.find("cdcd") == 3);
    REQUIRE(nfa.find("ac") == NFA::NoMatch);
    REQUIRE(nfa.find("ca") == NFA::NoMatch);
  }
}

TEST_CASE("NFA::MultipleRegex") {
  NFA nfa;
  nfa.addRegex("abcd+");
  nfa.addRegex("vwx?y");
  size_t reg;
  REQUIRE(nfa.find(reg, "abcd") == 3);
  REQUIRE(reg == 0);
  REQUIRE(nfa.find(reg, "abcddd") == 5);
  REQUIRE(reg == 0);
  REQUIRE(nfa.find(reg, "def") == NFA::NoMatch);
  REQUIRE(nfa.find(reg, "vwxy") == 3);
  REQUIRE(reg == 1);
  REQUIRE(nfa.find(reg, "vwy") == 2);
  REQUIRE(reg == 1);
}

TEST_CASE("NFA::ctor") {
  NFA nfa("abcd+");
  REQUIRE(nfa.find("abcd") == 3);
  REQUIRE(nfa.find("abcddd") == 5);
  REQUIRE(nfa.find("def") == NFA::NoMatch);
}

} // end namespace parser
} // end namespace teditor
