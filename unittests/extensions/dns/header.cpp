#include "extensions/dns/header.h"
#include "catch.hpp"

namespace teditor {
namespace dns {

TEST_CASE("SizeOfHeader") {
  REQUIRE(sizeof(Header) == 6 * sizeof(uint16_t));
}

TEST_CASE("Conversion") {
  auto ref = Header::Query();
  char buff[sizeof(Header)];
  REQUIRE(ref.serialize(buff) == sizeof(Header));
  Header obtained;
  REQUIRE(obtained.deserialize(buff) == sizeof(Header));
  REQUIRE(obtained == ref);
}

} // end namespace dns
} // end namespace teditor
