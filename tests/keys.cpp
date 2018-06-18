#include "keys.h"
#include "gtest/gtest.h"


namespace teditor {

TEST(Keys, MetaKey) {
    MetaKey mk;
    ASSERT_EQ(0xFFFF0000U, mk.getMeta());
    ASSERT_EQ(0xFFFFU, mk.getKey());
    ASSERT_TRUE(mk.isInvalid());
    ASSERT_EQ("", mk.toStr());

    MetaKey mk1(Meta_Ctrl | Key_a);
    ASSERT_EQ(Key_a, mk1.getKey());
    ASSERT_EQ(Meta_Ctrl, mk1.getMeta());
    ASSERT_FALSE(mk1.isInvalid());
    ASSERT_FALSE(mk1.isAscii());
    ASSERT_EQ("C-a", mk1.toStr());

    MetaKey mk2(Meta_None | Key_a);
    ASSERT_EQ(Key_a, mk2.getKey());
    ASSERT_EQ(Meta_None, mk2.getMeta());
    ASSERT_FALSE(mk2.isInvalid());
    ASSERT_TRUE(mk2.isAscii());
    ASSERT_EQ("a", mk2.toStr());

    MetaKey mk3(Meta_None, Key_a);
    ASSERT_EQ(Key_a, mk3.getKey());
    ASSERT_EQ(Meta_None, mk3.getMeta());
    ASSERT_FALSE(mk3.isInvalid());
    ASSERT_TRUE(mk3.isAscii());
    ASSERT_EQ("a", mk3.toStr());
    mk3.reset();
    ASSERT_EQ("", mk3.toStr());
    ASSERT_TRUE(mk3.isInvalid());
    mk3.setKey(Key_B);
    mk3.setMeta(Meta_None);
    ASSERT_EQ(Key_B, mk3.getKey());
    ASSERT_EQ("B", mk3.toStr());
    mk3.setMeta(Meta_None);
    ASSERT_EQ(Meta_None, mk3.getMeta());
    ASSERT_TRUE(mk3.isAscii());
    ASSERT_EQ("B", mk3.toStr());
    mk3.updateMeta(Meta_Alt);
    ASSERT_FALSE(mk3.isAscii());
    ASSERT_EQ("M-B", mk3.toStr());

    MetaKey mk4(Meta_None, Key_F1);
    ASSERT_EQ(Key_F1, mk4.getKey());
    ASSERT_EQ(Meta_None, mk4.getMeta());
    ASSERT_FALSE(mk4.isInvalid());
    ASSERT_FALSE(mk4.isAscii());
    ASSERT_EQ("F1", mk4.toStr());
}

} // end namespace teditor
