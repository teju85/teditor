#include "core/infocmp.h"
#include "core/terminal.h"
#include "catch.hpp"

namespace teditor {

TEST_CASE("InfoCmp") {
  InfoCmp ic;
  REQUIRE_NOTHROW(ic.getStrCap(func2terminfo(Func_EnterCA)));
  REQUIRE_NOTHROW(ic.getStrCap(func2terminfo(Func_ExitCA)));
  REQUIRE_NOTHROW(ic.getStrCap(func2terminfo(Func_ShowCursor)));
  REQUIRE_NOTHROW(ic.getStrCap(func2terminfo(Func_HideCursor)));
  REQUIRE_NOTHROW(ic.getStrCap(func2terminfo(Func_ClearScreen)));
  REQUIRE_NOTHROW(ic.getStrCap(func2terminfo(Func_Sgr0)));
  REQUIRE_NOTHROW(ic.getStrCap(func2terminfo(Func_Underline)));
  REQUIRE_NOTHROW(ic.getStrCap(func2terminfo(Func_Bold)));
  REQUIRE_NOTHROW(ic.getStrCap(func2terminfo(Func_Blink)));
  REQUIRE_NOTHROW(ic.getStrCap(func2terminfo(Func_EnterKeypad)));
  REQUIRE_NOTHROW(ic.getStrCap(func2terminfo(Func_ExitKeypad)));
}

}  // namespace teditor
