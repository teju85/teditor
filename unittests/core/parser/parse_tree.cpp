#include <catch.hpp>
#include "core/parser/parse_tree.h"
#include <sstream>

namespace teditor {
namespace parser {

TEST_CASE("printToken") {
  SECTION("Root") {
    Token t{Token::Root, Point{0, 0}, Point{0, 0}};
    std::ostringstream oss;
    oss << t;
    REQUIRE("Root: 0,0 to 0,0" == oss.str());
  }
  SECTION("End") {
    Token t{Token::End, Point{0, 0}, Point{0, 0}};
    std::ostringstream oss;
    oss << t;
    REQUIRE("End: 0,0 to 0,0" == oss.str());
  }
  SECTION("Unknown") {
    Token t{Token::Unknown, Point{0, 0}, Point{0, 0}};
    std::ostringstream oss;
    oss << t;
    REQUIRE("Unknown: 0,0 to 0,0" == oss.str());
  }
  SECTION("Others") {
    Token t{1u, Point{0, 0}, Point{0, 0}};
    std::ostringstream oss;
    oss << t;
    REQUIRE("1: 0,0 to 0,0" == oss.str());
  }
}

TEST_CASE("printNode") {
  Node* root = new Node(Token{Token::Root, Point{0, 0}, Point{0, 0}});

  SECTION("zero deep") {
    std::ostringstream oss;
    oss << *root;
    REQUIRE("(Root: 0,0 to 0,0)\n" == oss.str());
  }

  root->add(Token{0u, Point{0, 0}, Point{0, 0}});
  root->add(Token{1u, Point{0, 0}, Point{0, 0}});
  SECTION("one deep") {
    std::stringstream iss;
    iss << "(Root: 0,0 to 0,0)\n";
    iss << " |-(0: 0,0 to 0,0)\n";
    iss << " |-(1: 0,0 to 0,0)\n";
    std::ostringstream oss;
    oss << *root;
    REQUIRE(iss.str() == oss.str());
  }

  (*root)[0].add(Token{2u, Point{0, 0}, Point{0, 0}});
  SECTION("two deep") {
    std::stringstream iss;
    iss << "(Root: 0,0 to 0,0)\n";
    iss << " |-(0: 0,0 to 0,0)\n";
    iss << " | |-(2: 0,0 to 0,0)\n";
    iss << " |-(1: 0,0 to 0,0)\n";
    std::ostringstream oss;
    oss << *root;
    REQUIRE(iss.str() == oss.str());
  }

  (*root)[0][0].add(Token{3u, Point{0, 0}, Point{0, 0}});
  (*root)[0][0].add(Token{4u, Point{0, 0}, Point{0, 0}});
  (*root)[0][0].add(Token{5u, Point{0, 0}, Point{0, 0}});
  SECTION("three deep") {
    std::stringstream iss;
    iss << "(Root: 0,0 to 0,0)\n";
    iss << " |-(0: 0,0 to 0,0)\n";
    iss << " | |-(2: 0,0 to 0,0)\n";
    iss << " | | |-(3: 0,0 to 0,0)\n";
    iss << " | | |-(4: 0,0 to 0,0)\n";
    iss << " | | |-(5: 0,0 to 0,0)\n";
    iss << " |-(1: 0,0 to 0,0)\n";
    std::ostringstream oss;
    oss << *root;
    REQUIRE(iss.str() == oss.str());
  }

  delete root;
}

}  // namespace parser
}  // namespace teditor
