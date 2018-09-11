#include "args.h"
#include "utils.h"
#include "catch.hpp"

namespace teditor {

TEST_CASE("Args::Test") {
    Args ar(0, nullptr);
    auto home = env("HOME");
    REQUIRE(home + "/.teditor/test" == ar.wrtHomeFolder("test"));
}

} // end namespace teditor
