#include "buffer.h"
#include "gtest/gtest.h"

namespace teditor {

TEST(Buffer, Cell) {
    Cell c = {0, 0, 0};
    ASSERT_EQ(0, c.width());
    c.ch = 'c';
    ASSERT_EQ(1, c.width());
    Cell b;
    b.copy(c);
    ASSERT_EQ(c, b);
    b.set('b', 0, 0);
    ASSERT_FALSE(b == c);
}

TEST(Buffer, Line) {
    Line line;
    // empty line still needs a line!
    ASSERT_EQ(1, line.numLinesNeeded(5));
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
    line.erase(10);
    ASSERT_EQ(11, line.length());
    ASSERT_EQ("Hello Worl!", line.get());
    line.insert('d', 10);
    ASSERT_EQ(12, line.length());
    ASSERT_EQ("Hello World!", line.get());
    ASSERT_EQ(2, line.numLinesNeeded(6));
    ASSERT_EQ(3, line.numLinesNeeded(5));
    line.clear();
    ASSERT_EQ(0, line.length());
}

TEST(Buffer, MultiLineLocation) {
    MultiLine ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("tests/samples/multiline.txt", 2);
    const auto& pos = ml.getCursor().at(0);
    ASSERT_EQ(0, pos.x);
    ASSERT_EQ(2, pos.y);
    ASSERT_EQ("multiline.txt", ml.bufferName());
    ASSERT_EQ(4, ml.length());
}

TEST(Buffer, MultiLineKillLine) {
    MultiLine ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("tests/samples/multiline.txt", 2);
    const auto& pos = ml.getCursor().at(0);
    ASSERT_EQ(0, pos.x);
    ASSERT_EQ(2, pos.y);
    ASSERT_EQ("multiline.txt", ml.bufferName());
    ASSERT_EQ(4, ml.length());
    ml.killLine();
    ASSERT_EQ(4, ml.length());
    ml.killLine();
    ASSERT_EQ(3, ml.length());
}

TEST(Buffer, MultiLineBadFile) {
    MultiLine ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("tests/samples/nofile.txt", 2);
    const auto& pos = ml.getCursor().at(0);
    ASSERT_EQ(1, ml.length());
    ASSERT_EQ(0, pos.x);
    ASSERT_EQ(0, pos.y);
    ASSERT_EQ("nofile.txt", ml.bufferName());
}

TEST(Buffer, MultiLineDir) {
    MultiLine ml;
    ml.resize({0, 0}, {100, 10});
    ml.load("tests/samples");
    ASSERT_EQ(7, ml.length());
    ASSERT_EQ("samples", ml.bufferName());
}

TEST(Buffer, MultiLine) {
    MultiLine ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("tests/samples/multiline.txt");
    ASSERT_EQ(4, ml.length());
    ASSERT_EQ(1, ml.totalLinesNeeded());
    ASSERT_EQ('*', ml.charAt({0,0}));
    Pos2d<int> loc = {0, 0};
    Pos2d<int> bloc = ml.buffer2screen(loc);
    ASSERT_EQ(0, bloc.x);
    ASSERT_EQ(0, bloc.y);
    loc = {4, 1};
    ml.resize({0, 0}, {5, 10});
    ASSERT_EQ(2, ml.totalLinesNeeded());
    bloc = ml.buffer2screen(loc);
    ASSERT_EQ(4, bloc.x);
    ASSERT_EQ(2, bloc.y);
    loc = {5, 1};
    bloc = ml.buffer2screen(loc);
    ASSERT_EQ(0, bloc.x);
    ASSERT_EQ(3, bloc.y);
    loc = {9, 2};
    bloc = ml.buffer2screen(loc);
    ASSERT_EQ(4, bloc.x);
    ASSERT_EQ(5, bloc.y);
    ml.clear();
    ASSERT_EQ(1, ml.length());
}

TEST(Buffer, MultiLineInsert) {
    MultiLine ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("tests/samples/multiline.txt");
    Cursor& cu = ml.getCursor();
    cu.reset(&ml);
    cu.addBack(0, 1);
    ml.insert('T');
    ASSERT_EQ("T* Hello", ml.at(0).get());
    ASSERT_EQ("TTesting123", ml.at(1).get());
    ASSERT_EQ(2, cu.count());
    cu.clearAllButFirst();
    ASSERT_EQ(1, cu.count());
}

TEST(Buffer, MultiLineInsertLine) {
    MultiLine ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("tests/samples/multiline.txt");
    ASSERT_EQ(4, ml.length());
    auto& cu = ml.getCursor();
    // enter somewhere inside the line
    cu.at(0) = {0, 1};
    ml.insert('\n');
    ASSERT_EQ(5, ml.length());
    ASSERT_EQ("", ml.at(1).get());
    ASSERT_EQ("Testing123", ml.at(2).get());
    // enter at the end of line
    cu.at(0) = {ml.at(0).length(), 0};
    ml.insert('\n');
    ASSERT_EQ(6, ml.length());
    ASSERT_EQ("* Hello", ml.at(0).get());
    ASSERT_EQ("", ml.at(1).get());
}

TEST(Buffer, FindFirstNotOf) {
    MultiLine ml;
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

TEST(Buffer, FindLastNotOf) {
    MultiLine ml;
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

TEST(Buffer, Cursor) {
    MultiLine ml;
    ml.resize({0, 0}, {30, 10});
    Cursor c;
    ASSERT_TRUE(c.isHidden(0));
    ASSERT_TRUE(c.isHidden(0, Cursor::Hidden));
    ASSERT_TRUE(c.isHidden(Cursor::Hidden, 0));
    ASSERT_EQ(1, c.count());
    c.reset(&ml);
    ASSERT_FALSE(c.isHidden(0));
    ASSERT_EQ(1, c.count());
    c.addBack(0, 1);
    ASSERT_FALSE(c.isHidden(1));
    ASSERT_EQ(2, c.count());
    c.remove(0);
    ASSERT_EQ(1, c.count());
    ASSERT_EQ(Pos2d<int>(0, 1), c.at(0));

    c.addBack(1, 2);
    c.addBack(2, 4);
    ASSERT_EQ(3, c.count());
    c.reset(&ml);
    ASSERT_EQ(1, c.count());
    ASSERT_EQ(Pos2d<int>(0, 0), c.at(0));
}

} // end namespace teditor
