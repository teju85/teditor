#include "isearch.h"
#include "buffer.h"
#include "gtest/gtest.h"

namespace teditor {

TEST(ISearch, Usage) {
    MultiLine ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("tests/samples/multiline.txt");
    ASSERT_EQ(4, ml.length());
    ISearch is(ml);
    is.reset();
    ASSERT_TRUE(is.emptyAt(0));
    {
        is.addChar('e');
        ASSERT_FALSE(is.emptyAt(0));
        const auto& m0 = is.at(0);
        ASSERT_EQ(1U, m0.size());
        ASSERT_EQ(3, m0[0]);
        ASSERT_FALSE(is.emptyAt(1));
        const auto& m1 = is.at(1);
        ASSERT_EQ(1U, m1.size());
        ASSERT_FALSE(is.emptyAt(2));
        const auto& m2 = is.at(2);
        ASSERT_EQ(2U, m2.size());
        ASSERT_TRUE(is.emptyAt(3));
        ASSERT_THROW(is.at(3), std::runtime_error);
    } {
        is.addChar('l');
        ASSERT_FALSE(is.emptyAt(0));
        const auto& m0 = is.at(0);
        ASSERT_EQ(1U, m0.size());
        ASSERT_EQ(3, m0[0]);
        ASSERT_TRUE(is.emptyAt(1));
        ASSERT_TRUE(is.emptyAt(2));
    } {
        is.removeLast();
        ASSERT_FALSE(is.emptyAt(0));
        const auto& m0 = is.at(0);
        ASSERT_EQ(1U, m0.size());
        ASSERT_EQ(3, m0[0]);
        ASSERT_FALSE(is.emptyAt(1));
        const auto& m1 = is.at(1);
        ASSERT_EQ(1U, m1.size());
        ASSERT_FALSE(is.emptyAt(2));
        const auto& m2 = is.at(2);
        ASSERT_EQ(2U, m2.size());
    } {
        is.addChar('s');
        ASSERT_TRUE(is.emptyAt(0));
        ASSERT_FALSE(is.emptyAt(1));
        ASSERT_TRUE(is.emptyAt(2));
    }
}

} // end namespace teditor
