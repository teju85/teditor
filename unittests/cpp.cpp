#include "buffer.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("CppMode") {
    Buffer ml;
    ml.resize({0, 0}, {30, 10});
    ml.load("samples/sample.cxx", 0);
    REQUIRE("c++" == ml.modeName());
}

} // end namespace teditor
