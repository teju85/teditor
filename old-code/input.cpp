#include "event.h"
#include "gtest/gtest.h"


namespace teditor {

TEST(Click, Test) {
    Click c1(Key_MouseRelease);
    ASSERT_FALSE(c1.isDoubleClick());
    Click c2(Key_MouseRelease);
    ASSERT_TRUE(c2.isDoubleClick());
    Click c3(Key_None);
    ASSERT_FALSE(c3.isDoubleClick());
}

} // end namespace teditor
