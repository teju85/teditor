#include "buffer.h"
#include "gtest/gtest.h"

namespace teditor {

TEST(Buffer, Location) {
    Buffer ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("tests/samples/multiline.txt", 2);
    const auto& pos = ml.getCursor().at(0);
    ASSERT_EQ(0, pos.x);
    ASSERT_EQ(2, pos.y);
    ASSERT_EQ("multiline.txt", ml.bufferName());
    ASSERT_EQ(4, ml.length());
}

TEST(Buffer, KillLine) {
    Buffer ml;
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

TEST(Buffer, BadFile) {
    Buffer ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("tests/samples/nofile.txt", 2);
    const auto& pos = ml.getCursor().at(0);
    ASSERT_EQ(1, ml.length());
    ASSERT_EQ(0, pos.x);
    ASSERT_EQ(0, pos.y);
    ASSERT_EQ("nofile.txt", ml.bufferName());
}

TEST(Buffer, DirMode) {
    Buffer ml;
    ml.resize({0, 0}, {100, 10});
    ml.load("tests/samples");
    ASSERT_EQ(7, ml.length());
    ASSERT_EQ("samples", ml.bufferName());
}

TEST(Buffer, Buffer2screen) {
    Buffer ml;
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

TEST(Buffer, Insert) {
    Buffer ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("tests/samples/multiline.txt");
    Cursor& cu = ml.getCursor();
    cu.reset(&ml);
    cu.addBack(0, 1);
    ml.insert('T');
    ASSERT_EQ("T* Hello", ml.at(0).get());
    ASSERT_EQ("TTesting123", ml.at(1).get());
    ASSERT_EQ(2, cu.count());

    // inserting strings at multiple-cursors
    ml.insert("AA");
    ASSERT_EQ("TAA* Hello", ml.at(0).get());
    ASSERT_EQ("TAATesting123", ml.at(1).get());

    cu.clearAllButFirst();
    ASSERT_EQ(1, cu.count());

    // adding bunch of chars at the same time
    std::vector<std::string> strs;
    strs.push_back("line1");
    ASSERT_NO_THROW(ml.insert(strs));
    ASSERT_EQ("TAAline1* Hello", ml.at(0).get());

    // mismatch in number of cursors
    strs.push_back("line2");
    ASSERT_THROW(ml.insert(strs), std::runtime_error);
}

TEST(Buffer, InsertLine) {
    Buffer ml;
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

} // end namespace teditor
