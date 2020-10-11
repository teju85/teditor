#include "core/editor.h"
#include "core/command.h"
#include "core/option.h"
#include "parser.h"
#include "mode.h"

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

DEF_CMD(
  Calc, "calc", DEF_OP() {
    auto& buf = getCalcBuff(ed);
    printHeader(buf);
    ed.switchToBuff("*calc");
  },
  DEF_HELP() { return "Starts the calculator, if not already done."; });

DEF_CMD(
  InsertChar, ".calc::insert-char", DEF_OP() {
    insertChar(ed.getBuff(), (char)ed.getKey(), ed);
  },
  DEF_HELP() { return "Inserts currently pressed char into buffer."; });

DEF_CMD(
  Evaluate, "calc::enter", DEF_OP() {
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
    parser.evaluate(expr, vars());
    //@todo: fix this
    buf.insert(format("\nResult: %s\n", expr.c_str()));
    printHeader(buf);  // insert the next prompt
  },
  DEF_HELP() { return "Evaluate the current expression"; });

DEF_CMD(
  BackspaceChar, "calc::backspace-char", DEF_OP() {
    auto& buf = ed.getBuff();
    if (buf.isRegionActive()) buf.stopRegion();
    if (isOnPrompt(buf, true) || isOnSeparator(buf)) {
      CMBAR_MSG(ed, "Cannot delete the prompt or separator!");
      return;
    }
    buf.remove();
  },
  DEF_HELP() {
    return "Removes the char to the left of cursor, if it is not on prompt";
  });

DEF_CMD(
  DeleteChar, "calc::delete-char", DEF_OP() {
    auto& buf = ed.getBuff();
    if (buf.isRegionActive()) buf.stopRegion();
    if (isOnPrompt(buf, false) || isOnSeparator(buf)) {
      CMBAR_MSG(ed, "Cannot delete the prompt or separator!");
      return;
    }
    buf.remove(true);
  },
  DEF_HELP() {
    return "Removes the char on the cursor, if it is not on prompt";
  });

DEF_CMD(
  KillLine, "calc::kill-line", DEF_OP() {
    auto& buf = ed.getBuff();
    if (buf.isRegionActive()) buf.stopRegion();
    if (isOnPrompt(buf, false) || isOnSeparator(buf)) {
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
