#include "line.h"
#include "gtest/gtest.h"


namespace teditor {

TEST(Line, Line) {
    Line line;
    // empty line still needs a line!
    ASSERT_EQ(1, line.numLinesNeeded(5));
    ASSERT_TRUE(line.empty());
    ASSERT_EQ(0, line.length());
    ASSERT_EQ("", line.get());
    line.append('l');
    ASSERT_EQ(1, line.length());
    ASSERT_EQ("l", line.get());
    line.append("o !");
    ASSERT_EQ(4, line.length());
    ASSERT_EQ("lo !", line.get());
    line.prepend('l');
    ASSERT_EQ(5, line.length());
    ASSERT_EQ("llo !", line.get());
    line.prepend("He");
    ASSERT_EQ(7, line.length());
    ASSERT_EQ("Hello !", line.get());
    line.insert("Worlk", 6);
    ASSERT_EQ(12, line.length());
    ASSERT_EQ("Hello Worlk!", line.get());
    ASSERT_FALSE(line.empty());
    line.erase(10);
    ASSERT_EQ(11, line.length());
    ASSERT_EQ("Hello Worl!", line.get());
    line.insert('d', 10);
    ASSERT_EQ(12, line.length());
    ASSERT_EQ("Hello World!", line.get());
    ASSERT_EQ('W', line.at(6));
    ASSERT_EQ(2, line.numLinesNeeded(6));
    ASSERT_EQ(3, line.numLinesNeeded(5));
    line.clear();
    ASSERT_EQ(0, line.length());
    ASSERT_TRUE(line.empty());

    line.append("Hello World!");
    ASSERT_EQ(2, line.findFirstNotOf("abcdefghABCDEFGH", 0));
    ASSERT_EQ(line.length(), line.findFirstNotOf("abcdefghABCDEFGH", line.length()));
    ASSERT_EQ(line.length()-1, line.findLastNotOf("abcdefghABCDEFGH", line.length()));
    ASSERT_EQ(0, line.findLastNotOf("abcdefghABCDEFGH", 0));
    ASSERT_EQ(0, line.findLastNotOf("HelorWd !", line.length()));
}

} // end namespace teditor
