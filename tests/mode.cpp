#include "mode.h"
#include "gtest/gtest.h"

namespace teditor {

TEST(Mode, CreateMode) {
    ASSERT_THROW(Mode::createMode("not-exist"), std::runtime_error);
    ASSERT_EQ("text", Mode::createMode("text")->name());
}

TEST(Mode, InferMode) {
    ASSERT_EQ("text", Mode::inferMode("no-match"));
    ASSERT_EQ("dir", Mode::inferMode("src"));
    ASSERT_EQ("text", Mode::inferMode("file.txt"));
}

} // end namespace teditor
