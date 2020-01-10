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
    auto* mode = buf.getMode<CalcMode>("calc");
    mode->printHeader(buf);
    ed.switchToBuff("*calc");
  },
  DEF_HELP() { return "Starts the calculator, if not already done."; });

DEF_CMD(
  InsertChar, "calc-insert-char", DEF_OP() {
    auto& buf = ed.getBuff();
    auto* mode = buf.getMode<CalcMode>("calc");
    mode->insertChar(buf, (char)ed.getKey(), ed);
  },
  DEF_HELP() { return "Inserts currently pressed char into buffer."; });

DEF_CMD(
  Evaluate, "calc-enter", DEF_OP() {
    auto& buf = ed.getBuff();
    auto* mode = buf.getMode<CalcMode>("calc");
    mode->evaluate(buf, ed);
  },
  DEF_HELP() { return "Evaluate the current expression"; });

DEF_CMD(
  BackspaceChar, "calc-backspace-char", DEF_OP() {
    auto& buf = ed.getBuff();
    auto* mode = buf.getMode<CalcMode>("calc");
    if (buf.isRegionActive()) buf.stopRegion();
    if (mode->isOnPrompt(buf, true) || mode->isOnSeparator(buf)) {
      CMBAR_MSG(ed, "Cannot delete the prompt or separator!");
      return;
    }
    buf.remove();
  },
  DEF_HELP() {
    return "Removes the char to the left of cursor, if it is not on prompt";
  });

DEF_CMD(
  DeleteChar, "calc-delete-char", DEF_OP() {
    auto& buf = ed.getBuff();
    auto* mode = buf.getMode<CalcMode>("calc");
    if (buf.isRegionActive()) buf.stopRegion();
    if (mode->isOnPrompt(buf, false) || mode->isOnSeparator(buf)) {
      CMBAR_MSG(ed, "Cannot delete the prompt or separator!");
      return;
    }
    buf.remove(true);
  },
  DEF_HELP() {
    return "Removes the char on the cursor, if it is not on prompt";
  });

DEF_CMD(
  KillLine, "calc-kill-line", DEF_OP() {
    auto& buf = ed.getBuff();
    auto* mode = buf.getMode<CalcMode>("calc");
    if (buf.isRegionActive()) buf.stopRegion();
    if (mode->isOnPrompt(buf, false) || mode->isOnSeparator(buf)) {
      CMBAR_MSG(ed, "Cannot kill-line at the prompt or separator!");
      return;
    }
    auto del = buf.killLine();
    ed.setClipboard(del);
    CMBAR_MSG(ed, "Line killed\n");
  },
  DEF_HELP() {
    return "Removes the rest of the line and copies it to the clipboard.";
  });

} // end namespace ops
} // end namespace calc
} // end namespace teditor
