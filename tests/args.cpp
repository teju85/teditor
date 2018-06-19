#include "args.h"
#include "utils.h"
#include "gtest/gtest.h"

namespace teditor {

TEST(Args, Test) {
    Args ar(0, nullptr);
    auto home = env("HOME");
    ASSERT_EQ(home + "/.teditor/test", ar.wrtHomeFolder("test"));
}

} // end namespace teditor
