#include "trie.h"
#include "gtest/gtest.h"

namespace teditor {

TEST(Trie, Test) {
    Trie tr;
    tr.add("C-a C-x", "cmd");
    ASSERT_EQ(nullptr, tr.getNode(""));
    Node* n = tr.getNode("C-a");
    ASSERT_NE(n, nullptr);
    n = n->getNode("C-x");
    ASSERT_NE(n, nullptr);
    ASSERT_TRUE(n->leaf());
    ASSERT_EQ("cmd", n->data());
    ASSERT_NO_THROW(tr.add("C-b C-a C-x", "cmd1"));
    ASSERT_THROW(tr.add("C-a", "something"), std::runtime_error);
    ASSERT_NO_THROW(tr.add("C-b C-a C-x", "cmd2"));

    tr.clear();
    ASSERT_EQ(nullptr, tr.getNode("C-a"));
}

} // end namespace teditor
