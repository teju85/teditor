#include "cmd_msg_bar.h"
#include "gtest/gtest.h"


namespace teditor {

TEST(ChoicesFilter, StrFind) {
    ASSERT_TRUE(strFind("there is abc in this line", "abc"));
    ASSERT_FALSE(strFind("there is no", "abc"));
    ASSERT_FALSE(strFind("abc", "no matching"));
}

TEST(StringChoices, Test) {
    std::vector<std::string> opts = {
        "list",
        "options",
        "choices"
    };
    StringChoices sc(opts);
    ASSERT_EQ(3, sc.size());
    ASSERT_EQ("choices", sc.at(2));
    ASSERT_EQ("options", sc.getFinalStr(1, "dummy"));
}

TEST(CmdMsgBar, Default) {
    CmdMsgBar cmBar;
    cmBar.resize({0, 0}, {10, 10});
    ASSERT_EQ(1, cmBar.length());
    ASSERT_FALSE(cmBar.usingChoices());
    cmBar.insert("Test: ");
    cmBar.setMinLoc(6);
    ASSERT_EQ(6, cmBar.at(0).length());
    ASSERT_EQ("Test: ", cmBar.at(0).get());
    ASSERT_EQ(6, cmBar.getMinStartLoc());
    cmBar.insert('H');
    cmBar.insert('i');
    ASSERT_EQ(1, cmBar.totalLinesNeeded());
    ASSERT_EQ(8, cmBar.at(0).length());
    ASSERT_EQ("Test: Hi", cmBar.at(0).get());
    ASSERT_EQ("Hi", cmBar.getStr());
    ASSERT_EQ("Hi", cmBar.getFinalChoice());
    cmBar.setMinLoc(0);
    ASSERT_EQ("Test: Hi", cmBar.at(0).get());
    cmBar.clear();
    ASSERT_EQ(0, cmBar.at(0).length());
}

TEST(CmdMsgBar, UsingStringChoices) {
    std::vector<std::string> opts = {
        "list",
        "options",
        "choices"
    };
    StringChoices sc(opts);
    CmdMsgBar cmBar;
    cmBar.resize({0, 0}, {10, 10});
    ASSERT_EQ(1, cmBar.length());
    ASSERT_FALSE(cmBar.usingChoices());
    cmBar.insert("Test: ");
    cmBar.setMinLoc(6);
    ASSERT_EQ(6, cmBar.at(0).length());
    ASSERT_EQ("Test: ", cmBar.at(0).get());
    ASSERT_EQ(6, cmBar.getMinStartLoc());
    cmBar.setChoices(&sc);
    ASSERT_TRUE(cmBar.usingChoices());
    cmBar.insert('c');
    cmBar.insert('h');
    ASSERT_EQ(2, cmBar.totalLinesNeeded());
    ASSERT_EQ(8, cmBar.at(0).length());
    ASSERT_EQ("Test: ch", cmBar.at(0).get());
    ASSERT_EQ("ch", cmBar.getStr());
    ASSERT_EQ("choices", cmBar.getFinalChoice());
    ASSERT_EQ(2, cmBar.getOptLoc());
    cmBar.setMinLoc(0);
    ASSERT_EQ("Test: ch", cmBar.at(0).get());
    cmBar.clear();
    ASSERT_EQ(0, cmBar.at(0).length());
    cmBar.clearChoices();
    ASSERT_FALSE(cmBar.usingChoices());
}

} // end namespace teditor