#include "extensions/dns/header.h"
#include "catch.hpp"

namespace teditor {
namespace dns {

TEST_CASE("Header::SizeOfHeader") {
  REQUIRE(sizeof(Header) == 6 * sizeof(uint16_t));
}

TEST_CASE("Header::Conversion") {
  auto ref = Header::Query();
  char buff[sizeof(Header)];
  REQUIRE(ref.size() == sizeof(Header));
  ref.serialize(buff);
  Header obtained;
  obtained.deserialize(buff);
  REQUIRE(obtained == ref);
}

} // end namespace dns
} // end namespace teditor
