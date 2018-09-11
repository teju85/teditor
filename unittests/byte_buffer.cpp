#include "byte_buffer.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("ByteBuffer") {
    // initial state
    ByteBuffer b(128);
    REQUIRE(128U == b.capacity());
    REQUIRE(0U == b.length());
    REQUIRE("" == b);
    // adding data into the buffer
    b.puts("Hello");
    REQUIRE(128U == b.capacity());
    REQUIRE(5U == b.length());
    REQUIRE("Hello" == b);
    // After clearing the contents
    b.clear();
    REQUIRE(128U == b.capacity());
    REQUIRE(0U == b.length());
    REQUIRE("" == b);
    // append
    b.append("Hi", 2);
    REQUIRE(128U == b.capacity());
    REQUIRE(2U == b.length());
    REQUIRE("Hi" == b);
    // reserve (NOP)
    b.reserve(64);
    REQUIRE(128U == b.capacity());
    REQUIRE(2U == b.length());
    REQUIRE("Hi" == b);
    // reserve preferred
    b.reserve(150);
    REQUIRE(256U == b.capacity());
    REQUIRE(2U == b.length());
    REQUIRE("Hi" == b);
    // user provided reserve larger
    b.reserve(1024);
    REQUIRE(1024U == b.capacity());
    REQUIRE(2U == b.length());
    REQUIRE("Hi" == b);
}

} // end namespace teditor
