#include "isearch.h"
#include "buffer.h"
#include "gtest/gtest.h"

namespace teditor {

TEST(ISearch, Usage) {
    Buffer ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("tests/samples/multiline.txt");
    ASSERT_EQ(4, ml.length());
    ISearch is(ml);
    is.reset();
    ASSERT_TRUE(is.emptyAt(0));
    {
        ASSERT_FALSE(is.updateChoices(""));
    } {
        ASSERT_TRUE(is.updateChoices("e"));
        const auto& m0 = is.matchesAt(0);
        ASSERT_EQ(1U, m0.size());
        ASSERT_EQ(3, m0[0]);
        ASSERT_FALSE(is.emptyAt(1));
        const auto& m1 = is.matchesAt(1);
        ASSERT_EQ(1U, m1.size());
        ASSERT_FALSE(is.emptyAt(2));
        const auto& m2 = is.matchesAt(2);
        ASSERT_EQ(2U, m2.size());
        ASSERT_TRUE(is.emptyAt(3));
        ASSERT_THROW(is.matchesAt(3), std::runtime_error);
    } {
        ASSERT_TRUE(is.updateChoices("el"));
        ASSERT_FALSE(is.emptyAt(0));
        const auto& m0 = is.matchesAt(0);
        ASSERT_EQ(1U, m0.size());
        ASSERT_EQ(3, m0[0]);
        ASSERT_TRUE(is.emptyAt(1));
        ASSERT_TRUE(is.emptyAt(2));
    } {
        ASSERT_TRUE(is.updateChoices("e"));
        ASSERT_FALSE(is.emptyAt(0));
        const auto& m0 = is.matchesAt(0);
        ASSERT_EQ(1U, m0.size());
        ASSERT_EQ(3, m0[0]);
        ASSERT_FALSE(is.emptyAt(1));
        const auto& m1 = is.matchesAt(1);
        ASSERT_EQ(1U, m1.size());
        ASSERT_FALSE(is.emptyAt(2));
        const auto& m2 = is.matchesAt(2);
        ASSERT_EQ(2U, m2.size());
    } {
        ASSERT_TRUE(is.updateChoices("es"));
        ASSERT_TRUE(is.emptyAt(0));
        ASSERT_FALSE(is.emptyAt(1));
        ASSERT_TRUE(is.emptyAt(2));
    } {
        ASSERT_TRUE(is.updateChoices("e"));
        ASSERT_FALSE(is.updateChoices("e"));
        ASSERT_FALSE(is.emptyAt(0));
        const auto& m0 = is.matchesAt(0);
        ASSERT_EQ(1U, m0.size());
        ASSERT_EQ(3, m0[0]);
        ASSERT_FALSE(is.emptyAt(1));
        const auto& m1 = is.matchesAt(1);
        ASSERT_EQ(1U, m1.size());
        ASSERT_FALSE(is.emptyAt(2));
        const auto& m2 = is.matchesAt(2);
        ASSERT_EQ(2U, m2.size());
    } {
        ASSERT_TRUE(is.updateChoices(""));
        ASSERT_FALSE(is.updateChoices(""));
    }
}

} // end namespace teditor
