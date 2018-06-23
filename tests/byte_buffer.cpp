#include "gtest/gtest.h"
#include "byte_buffer.h"


namespace teditor {

TEST(ByteBuffer, ByteBuffer) {
    // initial state
    ByteBuffer b(128);
    ASSERT_EQ(128U, b.capacity());
    ASSERT_EQ(0U, b.length());
    ASSERT_EQ("", b);
    // adding data into the buffer
    b.puts("Hello");
    ASSERT_EQ(128U, b.capacity());
    ASSERT_EQ(5U, b.length());
    ASSERT_EQ("Hello", b);
    // After clearing the contents
    b.clear();
    ASSERT_EQ(128U, b.capacity());
    ASSERT_EQ(0U, b.length());
    ASSERT_EQ("", b);
    // append
    b.append("Hi", 2);
    ASSERT_EQ(128U, b.capacity());
    ASSERT_EQ(2U, b.length());
    ASSERT_EQ("Hi", b);
    // reserve (NOP)
    b.reserve(64);
    ASSERT_EQ(128U, b.capacity());
    ASSERT_EQ(2U, b.length());
    ASSERT_EQ("Hi", b);
    // reserve preferred
    b.reserve(150);
    ASSERT_EQ(256U, b.capacity());
    ASSERT_EQ(2U, b.length());
    ASSERT_EQ("Hi", b);
    // user provided reserve larger
    b.reserve(1024);
    ASSERT_EQ(1024U, b.capacity());
    ASSERT_EQ(2U, b.length());
    ASSERT_EQ("Hi", b);
}

} // end namespace teditor
