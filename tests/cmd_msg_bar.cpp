#include "cmd_msg_bar.h"
#include "gtest/gtest.h"


namespace teditor {

TEST(ChoicesFilter, StrFind) {
    ASSERT_TRUE(strFind("there is abc in this line", "abc"));
    ASSERT_FALSE(strFind("there is no", "abc"));
    ASSERT_FALSE(strFind("abc", "no matching"));
}

} // end namespace teditor
