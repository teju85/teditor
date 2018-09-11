#include "trie.h"
#include "catch.hpp"

namespace teditor {

TEST_CASE("Trie::Test") {
    Trie tr;
    tr.add("C-a C-x", "cmd");
    REQUIRE(nullptr == tr.getNode(""));
    Node* n = tr.getNode("C-a");
    REQUIRE(n != nullptr);
    n = n->getNode("C-x");
    REQUIRE(n != nullptr);
    REQUIRE(n->leaf());
    REQUIRE("cmd" == n->data());
    REQUIRE_NOTHROW(tr.add("C-b C-a C-x", "cmd1"));
    REQUIRE_THROWS(tr.add("C-a", "something"));
    REQUIRE_NOTHROW(tr.add("C-b C-a C-x", "cmd2"));

    tr.clear();
    REQUIRE(nullptr == tr.getNode("C-a"));
}

} // end namespace teditor
