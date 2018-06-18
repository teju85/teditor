#include "colors.h"
#include "gtest/gtest.h"

namespace teditor {

TEST(Colors, AttrColor) {
    AttrColor ac;
    ASSERT_FALSE(ac.isBold());
    ASSERT_FALSE(ac.isUnderline());
    ac.setBold();
    ASSERT_TRUE(ac.isBold());
    ac.setUnderline();
    ASSERT_TRUE(ac.isUnderline());
    AttrColor ac1, ac2;
    ASSERT_EQ(ac1, ac2);
    ASSERT_NE(ac, ac1);
    ac.clearAttr();
    ASSERT_EQ(ac, ac1);
    ac.setColor(Color_Red);
    ASSERT_EQ(Color_Red, ac.color());
}

TEST(Colors, AttrHelper) {
    ASSERT_THROW(AttrHelper::fromstr("Invalid"), std::runtime_error);
    ASSERT_EQ(Attr_Bold, AttrHelper::fromstr("Bold"));
    ASSERT_EQ("Bold", AttrHelper::tostr(Attr_Bold));
}

TEST(Colors, ColorHelper) {
    ASSERT_THROW(ColorHelper::fromstr("Invalid"), std::runtime_error);
    ASSERT_EQ(Color_Red, ColorHelper::fromstr("Red"));
    ASSERT_EQ("Red", ColorHelper::tostr(Color_Red));
}

} // end namespace teditor
