#include <catch.hpp>
#include "extensions/todo/parser.h"
#include "extensions/todo/objects.h"

namespace teditor {
namespace todo {

TEST_CASE("correct.todo") {
  Parser parser("samples/todo/correct.todo");
  REQUIRE(5 == parser.items().size());
  parser.reload();
  REQUIRE(5 == parser.items().size());
}

}  // namespace todo
}  // namespace teditor
