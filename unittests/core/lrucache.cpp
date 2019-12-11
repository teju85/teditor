#include "testutils.h"
#include "core/lrucache.hpp"
#include "catch.hpp"


namespace teditor {

TEST_CASE("LRUCache") {
  LRUCache<int, int> cache(5);
  REQUIRE(cache.capacity() == 5);
  SECTION("empty test") {
    REQUIRE(cache.size() == 0);
    REQUIRE_FALSE(cache.exists(1));
  }
  SECTION("put test") {
    cache.put(1, 11);
    REQUIRE(cache.size() == 1);
    REQUIRE(cache.exists(1));
  }
  SECTION("get test") {
    cache.put(1, 11);
    REQUIRE(cache.get(1) == 11);
  }
  SECTION("capacity test") {
    cache.put(1, 11);
    cache.put(2, 12);
    cache.put(3, 13);
    cache.put(4, 14);
    cache.put(5, 15);
    REQUIRE(cache.size() == 5);
    cache.put(6, 16);
    REQUIRE(cache.size() == 5);
    REQUIRE_FALSE(cache.exists(1));
    REQUIRE(cache.get(2) == 12);
    cache.put(5, 17);
    REQUIRE_FALSE(cache.exists(3));
  }
}

}  // namespace teditor
