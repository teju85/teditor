#include "files_hist.h"
#include "gtest/gtest.h"


namespace teditor {

TEST(FilesHist, FilesHist) {
    const std::string file("test_hist.txt");
    remove(file.c_str());

    FilesHist fh(file, 3);
    ASSERT_FALSE(isFile(file.c_str()));
    ASSERT_TRUE(fh.empty());
    auto vec = fh.toString();
    ASSERT_TRUE(vec.empty());

    // adding fresh one
    fh.add("main.cpp", 0);
    ASSERT_EQ(1U, fh.size());
    ASSERT_EQ(1U, fh.size());
    vec = fh.toString();
    ASSERT_FALSE(vec.empty());
    ASSERT_EQ("main.cpp:0", vec[0]);

    // adding duplicate
    fh.add("main.cpp", 1);
    vec = fh.toString();
    ASSERT_EQ(1U, fh.size());
    ASSERT_EQ("main.cpp:1", vec[0]);

    fh.add("tests", 0);
    vec = fh.toString();
    ASSERT_EQ(2U, fh.size());
    ASSERT_EQ("tests:0", vec[0]);

    fh.add("src", 0);
    vec = fh.toString();
    ASSERT_EQ(3U, fh.size());
    ASSERT_EQ("src:0", vec[0]);

    // crossing the limit
    fh.add("Doxyfile", 10);
    vec = fh.toString();
    ASSERT_EQ(3U, fh.size());
    ASSERT_EQ("Doxyfile:10", vec[0]);
    ASSERT_EQ("src:0", vec[1]);
    ASSERT_EQ("tests:0", vec[2]);

    // storage and reload it
    fh.store();
    ASSERT_TRUE(isFile(file.c_str()));
    FilesHist fh1(file, 2);
    vec = fh1.toString();
    ASSERT_EQ(2U, fh1.size());
    ASSERT_EQ("Doxyfile:10", vec[0]);
    ASSERT_EQ("src:0", vec[1]);

    remove(file.c_str());
}

} // end namespace teditor
