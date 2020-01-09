#include "core/key_cmd_map.h"
#include "catch.hpp"

namespace teditor {

TEST_CASE("KeyCmdMap::Usage") {
  KeyCmdMap kcm;
  kcm.add("C-a C-x", "cmd1");
  kcm.add("C-a C-b", "cmd2");
  kcm.add("C-b C-x", "cmd3");
  kcm.add("C-q", "cmd4");
  REQUIRE(TS_NULL == kcm.traverse("C-z"));
  REQUIRE(TS_NON_LEAF == kcm.traverse("C-b"));
  REQUIRE(TS_NULL == kcm.traverse("M-s"));
  REQUIRE(TS_NON_LEAF == kcm.traverse("C-b"));
  REQUIRE(TS_LEAF == kcm.traverse("C-x"));
  REQUIRE("cmd3" == kcm.getCmd());

  kcm.add("C-b C-x", "cmd5");
  kcm.resetTraversal();
  REQUIRE(TS_NON_LEAF == kcm.traverse("C-b"));
  REQUIRE(TS_LEAF == kcm.traverse("C-x"));
  REQUIRE("cmd5" == kcm.getCmd());

  kcm.add({"C-z C-y", "cmd6"});
  kcm.resetTraversal();
  REQUIRE(TS_NON_LEAF == kcm.traverse("C-z"));
  REQUIRE(TS_LEAF == kcm.traverse("C-y"));
  REQUIRE("cmd6" == kcm.getCmd());

  kcm.eraseKey("C-z C-y");
  kcm.resetTraversal();
  REQUIRE(TS_NULL == kcm.traverse("C-z"));
}

} // end namespace teditor
