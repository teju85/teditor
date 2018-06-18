#include "key_cmd_map.h"
#include "gtest/gtest.h"

namespace teditor {

TEST(KeyCmdMap, Usage) {
    KeyCmdMap kcm;
    kcm.add("C-a C-x", "cmd1");
    kcm.add("C-a C-b", "cmd2");
    kcm.add("C-b C-x", "cmd3");
    kcm.add("C-q", "cmd4");
    ASSERT_EQ(TS_NULL, kcm.traverse("C-z"));
    ASSERT_EQ(TS_NON_LEAF, kcm.traverse("C-b"));
    ASSERT_EQ(TS_NULL, kcm.traverse("M-s"));
    ASSERT_EQ(TS_NON_LEAF, kcm.traverse("C-b"));
    ASSERT_EQ(TS_LEAF, kcm.traverse("C-x"));
    ASSERT_EQ("cmd3", kcm.getCmd());

    kcm.add("C-b C-x", "cmd5");
    kcm.resetTraversal();
    ASSERT_EQ(TS_NON_LEAF, kcm.traverse("C-b"));
    ASSERT_EQ(TS_LEAF, kcm.traverse("C-x"));
    ASSERT_EQ("cmd5", kcm.getCmd());

    kcm.add({"C-z C-y", "cmd6"});
    kcm.resetTraversal();
    ASSERT_EQ(TS_NON_LEAF, kcm.traverse("C-z"));
    ASSERT_EQ(TS_LEAF, kcm.traverse("C-y"));
    ASSERT_EQ("cmd6", kcm.getCmd());
}

} // end namespace teditor
