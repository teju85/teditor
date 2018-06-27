#include "line.h"
#include "buffer.h"
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

TEST(Line, FindFirstNotOf) {
    Buffer ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("tests/samples/multiline.txt");
    const auto& line = ml.at(2);
    std::string word("abcdefghijklmnopqrstuvwxyzABCDEGGHIJKLMNOPQRSTUVWXYZ0123456789_");
    ASSERT_EQ(3, line.findFirstNotOf(word, 0));
    ASSERT_EQ(9, line.findFirstNotOf(word, 4));
    ASSERT_EQ(14, line.findFirstNotOf(word, 10));
    ASSERT_EQ(21, line.findFirstNotOf(word, 15));
    ASSERT_EQ(22, line.findFirstNotOf(word, 22));
}

TEST(Line, FindLastNotOf) {
    Buffer ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("tests/samples/multiline.txt");
    const auto& line = ml.at(2);
    std::string word("abcdefghijklmnopqrstuvwxyzABCDEGGHIJKLMNOPQRSTUVWXYZ0123456789_");
    ASSERT_EQ(21, line.findLastNotOf(word, 22));
    ASSERT_EQ(14, line.findLastNotOf(word, 20));
    ASSERT_EQ(9, line.findLastNotOf(word, 13));
    ASSERT_EQ(3, line.findLastNotOf(word, 8));
    ASSERT_EQ(0, line.findLastNotOf(word, 2));
}

TEST(Line, IndentSize) {
    Buffer ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("tests/samples/sample.cxx");
    ASSERT_EQ(0, ml.at(0).indentSize());
    ASSERT_EQ(0, ml.at(3).indentSize());
    ASSERT_EQ(4, ml.at(18).indentSize());
}

TEST(Line, LineCompare) {
    Line line1;
    line1.append("Test");
    Line line2;
    line2.append("test");
    ASSERT_TRUE(LineCompare(line1, line2));
    ASSERT_FALSE(LineCompare(line2, line2));
}

} // end namespace teditor
