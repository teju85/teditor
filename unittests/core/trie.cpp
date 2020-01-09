#include "core/trie.h"
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

  tr.add("C-a C-x", "cmd1");
  tr.add("C-a C-y", "cmd2");
  tr.add("C-b", "cmd3");
  tr.add(" ", "cmd4");
  {
    REQUIRE(nullptr != tr.getNode(" "));
    tr.del(" ");
    REQUIRE(nullptr == tr.getNode(" "));
  } {
    REQUIRE(nullptr != tr.getNode("C-b"));
    tr.del("C-b");
    REQUIRE(nullptr == tr.getNode("C-b"));
  } {
    // C-a C-x
    n = tr.getNode("C-a");
    REQUIRE(nullptr != n);
    n = n->getNode("C-x");
    REQUIRE(nullptr != n);
    // C-a C-y
    n = tr.getNode("C-a");
    REQUIRE(nullptr != n);
    n = n->getNode("C-y");
    REQUIRE(nullptr != n);
    // remove C-a C-y, but can still find C-a C-x
    tr.del("C-a C-y");
    // C-a C-x
    n = tr.getNode("C-a");
    REQUIRE(nullptr != n);
    n = n->getNode("C-x");
    REQUIRE(nullptr != n);
    // C-a C-y will not be found
    n = tr.getNode("C-a");
    REQUIRE(nullptr != n);
    REQUIRE(nullptr == n->getNode("C-y"));
    // remove C-a C-x, now even C-a should not be found
    tr.del("C-a C-x");
    REQUIRE(nullptr == n->getNode("C-a"));
  }
}

} // end namespace teditor
