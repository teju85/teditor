#include "core/infocmp.h"
#include "core/terminal.h"
#include "catch.hpp"

namespace teditor {

TEST_CASE("InfoCmp") {
  InfoCmp ic;
  for (int i = 0; i < Func_FuncsNum - 2; ++i) {
    REQUIRE_NOTHROW(ic.getStrCap(func2terminfo(Func(i))));
  }
}

}  // namespace teditor
