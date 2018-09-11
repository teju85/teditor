#include "line.h"
#include "buffer.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("Line::Line") {
    Line line;
    // empty line still needs a line!
    REQUIRE(1 == line.numLinesNeeded(5));
    REQUIRE(line.empty());
    REQUIRE(0 == line.length());
    REQUIRE("" == line.get());
    line.append('l');
    REQUIRE(1 == line.length());
    REQUIRE("l" == line.get());
    line.append("o !");
    REQUIRE(4 == line.length());
    REQUIRE("lo !" == line.get());
    line.prepend('l');
    REQUIRE(5 == line.length());
    REQUIRE("llo !" == line.get());
    line.prepend("He");
    REQUIRE(7 == line.length());
    REQUIRE("Hello !" == line.get());
    line.insert("Worlk", 6);
    REQUIRE(12 == line.length());
    REQUIRE("Hello Worlk!" == line.get());
    REQUIRE_FALSE(line.empty());
    line.erase(10);
    REQUIRE(11 == line.length());
    REQUIRE("Hello Worl!" == line.get());
    line.insert('d', 10);
    REQUIRE(12 == line.length());
    REQUIRE("Hello World!" == line.get());
    REQUIRE('W' == line.at(6));
    REQUIRE(2 == line.numLinesNeeded(6));
    REQUIRE(3 == line.numLinesNeeded(5));
    line.clear();
    REQUIRE(0 == line.length());
    REQUIRE(line.empty());

    line.append("Hello World!");
    REQUIRE(2 == line.findFirstNotOf("abcdefghABCDEFGH", 0));
    REQUIRE(line.length() == line.findFirstNotOf("abcdefghABCDEFGH", line.length()));
    REQUIRE(line.length()-1 == line.findLastNotOf("abcdefghABCDEFGH", line.length()));
    REQUIRE(0 == line.findLastNotOf("abcdefghABCDEFGH", 0));
    REQUIRE(0 == line.findLastNotOf("HelorWd !", line.length()));
}

TEST_CASE("Line::FindFirstNotOf") {
    Buffer ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("samples/multiline.txt");
    const auto& line = ml.at(2);
    std::string word("abcdefghijklmnopqrstuvwxyzABCDEGGHIJKLMNOPQRSTUVWXYZ0123456789_");
    REQUIRE(3 == line.findFirstNotOf(word, 0));
    REQUIRE(9 == line.findFirstNotOf(word, 4));
    REQUIRE(14 == line.findFirstNotOf(word, 10));
    REQUIRE(21 == line.findFirstNotOf(word, 15));
    REQUIRE(22 == line.findFirstNotOf(word, 22));
}

TEST_CASE("Line::FindLastNotOf") {
    Buffer ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("samples/multiline.txt");
    const auto& line = ml.at(2);
    std::string word("abcdefghijklmnopqrstuvwxyzABCDEGGHIJKLMNOPQRSTUVWXYZ0123456789_");
    REQUIRE(21 == line.findLastNotOf(word, 22));
    REQUIRE(14 == line.findLastNotOf(word, 20));
    REQUIRE(9 == line.findLastNotOf(word, 13));
    REQUIRE(3 == line.findLastNotOf(word, 8));
    REQUIRE(0 == line.findLastNotOf(word, 2));
}

TEST_CASE("Line::IndentSize") {
    Buffer ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("samples/sample.cxx");
    REQUIRE(0 == ml.at(0).indentSize());
    REQUIRE(0 == ml.at(3).indentSize());
    REQUIRE(4 == ml.at(18).indentSize());
}

TEST_CASE("Line::LineCompare") {
    Line line1;
    line1.append("Test");
    Line line2;
    line2.append("test");
    REQUIRE(LineCompare(line1, line2));
    REQUIRE_FALSE(LineCompare(line2, line2));
}

} // end namespace teditor