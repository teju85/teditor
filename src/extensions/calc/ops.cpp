#include "core/editor.h"
#include "core/command.h"
#include "core/isearch.h"
#include "mode.h"
#include "core/option.h"

namespace teditor {
namespace calc {
namespace ops {

Buffer& getCalcBuff(Editor& ed) {
  bool newOne;
  auto& buf = ed.getBuff("*calc", true, newOne);
  if (newOne)
    buf.setMode(Mode::createMode(Mode::inferMode(buf.bufferName(), false)));
  return buf;
}

DEF_CMD(
  Calc, "calc", DEF_OP() {
    auto& buf = getCalcBuff(ed);
    buf.insert(Option::get("calc:prompt").getStr());
    ed.switchToBuff("*calc");
  },
  DEF_HELP() { return "Starts the calculator, if not already done."; });

DEF_CMD(
  InsertChar, "calc-insert-char", DEF_OP() {
    auto& buf = ed.getBuff();
    auto c = (char)ed.getKey();
    buf.insert(c);
  },
  DEF_HELP() { return "Inserts currently pressed char into buffer."; });

} // end namespace ops
} // end namespace calc
} // end namespace teditor
