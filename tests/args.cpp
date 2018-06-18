#include "args.h"
#include "gtest/gtest.h"

namespace teditor {

TEST(Args, ReadFileInfo) {
    Args ar(0, nullptr);
    FileInfo fi = ar.readFileInfo("README.org:10");
    ASSERT_EQ("README.org", fi.first);
    ASSERT_EQ(10, fi.second);
    ASSERT_THROW(ar.readFileInfo("README.org:10:10"), std::runtime_error);
}

} // end namespace teditor
