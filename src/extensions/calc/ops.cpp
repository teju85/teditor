#include "core/editor.h"
#include "core/command.h"
#include "core/option.h"
#include "parser.h"
#include "mode.h"

namespace teditor {
namespace calc {
namespace ops {

/**
 * @page calc_ops calc-mode
 * All operations that are supported while under `calc-mode`
 *
 * @tableofcontents
 *
 *
 * @section calc
 * Starts the calculator under `calc-mode`, if not already done.
 *
 * @note Available since v1.7.0
 *
 *
 * @section calc_history calc::history
 * Opens a prompt with a list of previoulsy entered commands to quickly search
 * and jump to the command of interest to be shown in the current prompt instead
 *
 * @note Available since v1.7.0
 *
 *
 * @section calc_insert-char .calc::insert-char
 * Inserts currently pressed char into buffer, at the current calculator line.
 *
 * @note Available since v1.7.0
 *
 * @note Preceeding `.` implies that this command is not searchable from the
 *        editor command prompt.
 *
 *
 * @section calc_enter calc::enter
 * Evaluate the current expression and print the result below it.
 * 
 * @note Available since v1.7.0
 *
 *
 * @section calc_backspace-char calc::backspace-char
 * Removes the char to the left of the cursor, from the current line.
 * 
 * @note Available since v1.7.0
 *
 *
 * @section calc_delete-char calc::delete-char
 * Removes the char on the cursor, from the current line.
 *
 * @note Available since v1.7.0
 *
 *
 * @section calc_kill-line calc::kill-line
 * Removes the rest of the line and copies it to the clipboard.
 * 
 * @note Available since v1.7.0
 *
 * @note Currently editor has a notion of clipboard that is separate from the
 *        OS provided system clipboard!
 */

Buffer& getCalcBuff(Editor& ed) {
  bool newOne;
  auto& buf = ed.getBuff("*calc", true, newOne);
  if (newOne)
    buf.setMode(Mode::createMode(Mode::inferMode(buf.bufferName(), false)));
  return buf;
}

const std::string& prompt() {
  static std::string p(Option::get("calc:prompt").getStr());
  return p;
}

const std::string& lineSeparator() {
  static std::string ls(Option::get("calc:lineSeparator").getStr());
  return ls;
}

bool isPromptLine(Buffer& buf) {
  const auto& pt = buf.getPoint();
  const auto& line = buf.at(pt.y).get();
  const auto& p = prompt();
  return line.compare(0, p.size(), p) == 0;
}

bool isOnPrompt(Buffer& buf, bool includeTrailingSpace = false) {
  if (!isPromptLine(buf)) return false;
  const auto& pt = buf.getPoint();
  auto len = int(prompt().size());
  if (includeTrailingSpace) return pt.x <= len;
  return pt.x < len;
}

bool isOnSeparator(Buffer& buf) {
  const auto& pt = buf.getPoint();
  const auto& line = buf.at(pt.y).get();
  return line == lineSeparator();
}

void printHeader(Buffer& buf) {
  buf.end();
  if (isPromptLine(buf)) return;
  buf.insert(lineSeparator() + '\n');
  buf.insert(prompt());
}

void insertChar(Buffer& buf, char c, Editor& ed) {
  if (isPromptLine(buf)) {
    if (isOnPrompt(buf)) CMBAR_MSG(ed, "Cannot insert on prompt!\n");
    else buf.insert(c);
  } else {
    CMBAR_MSG(ed, "Cannot insert on a non-prompt line!\n");
  }
}


DEF_CMD(Calc, "calc", "calc_ops", DEF_OP() {
    auto& buf = getCalcBuff(ed);
    printHeader(buf);
    ed.switchToBuff("*calc");
  });

DEF_CMD(CommandHistory, "calc::history", "calc_ops", DEF_OP() {
    StringChoices sc(cmds().get());
    auto cmd = ed.prompt("Calc Command History: ", nullptr, &sc);
    if(cmd.empty()) return;
    auto& buf = ed.getBuff();
    buf.startOfLine();
    buf.killLine();
    buf.insert(prompt());
    buf.insert(cmd);
  });

DEF_CMD(InsertChar, ".calc::insert-char", "calc_ops",
        DEF_OP() { insertChar(ed.getBuff(), (char)ed.getKey(), ed); });

DEF_CMD(Evaluate, "calc::enter", "calc_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    Parser parser;
    const auto& pt = buf.getPoint();
    const auto& line = buf.at(pt.y).get();
    const auto& p = prompt();
    if (line.compare(0, p.size(), p) != 0) {
      CMBAR_MSG(ed, "Cannot evaluate a non-expr line!\n");
      return;
    }
    auto expr = line.substr(p.size());
    if (expr.empty()) return;
    addCmd(expr);
    parser.evaluate(expr, vars());
    //@todo: fix this
    buf.insert(format("\nResult: %s\n", expr.c_str()));
    printHeader(buf);  // insert the next prompt
  });

DEF_CMD(BackspaceChar, "calc::backspace-char", "calc_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    if (buf.isRegionActive()) buf.stopRegion();
    if (isOnPrompt(buf, true) || isOnSeparator(buf)) {
      CMBAR_MSG(ed, "Cannot delete the prompt or separator!");
      return;
    }
    buf.remove();
  });

DEF_CMD(DeleteChar, "calc::delete-char", "calc_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    if (buf.isRegionActive()) buf.stopRegion();
    if (isOnPrompt(buf, false) || isOnSeparator(buf)) {
      CMBAR_MSG(ed, "Cannot delete the prompt or separator!");
      return;
    }
    buf.remove(true);
  });

DEF_CMD(KillLine, "calc::kill-line", "calc_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    if (buf.isRegionActive()) buf.stopRegion();
    if (isOnPrompt(buf, false) || isOnSeparator(buf)) {
      CMBAR_MSG(ed, "Cannot kill-line at the prompt or separator!");
      return;
    }
    auto del = buf.killLine();
    ed.setClipboard(del);
    CMBAR_MSG(ed, "Line killed\n");
  });

} // end namespace ops
} // end namespace calc
} // end namespace teditor
