#include "core/editor.h"
#include "core/command.h"


namespace teditor {
namespace CursorOps {

DEF_CMD(CursorDown, "cursor-down",
        DEF_OP() { ed.getBuff().down(); },
        DEF_HELP() { return "Move cursor(s) a line down"; });

DEF_CMD(CursorUp, "cursor-up",
        DEF_OP() { ed.getBuff().up(); },
        DEF_HELP() { return "Move cursor(s) a line up"; });

DEF_CMD(CursorRight, "cursor-right",
        DEF_OP() { ed.getBuff().right(); },
        DEF_HELP() { return "Move cursor(s) a char to right"; });

DEF_CMD(CursorLeft, "cursor-left",
        DEF_OP() { ed.getBuff().left(); },
        DEF_HELP() { return "Move cursor(s) a char to left"; });

DEF_CMD(CursorReset, "cursor-reset",
        DEF_OP() { ed.getBuff().begin(); },
        DEF_HELP() { return "Go to start of the buffer"; });

DEF_CMD(
  CursorEnd, "cursor-end", DEF_OP() {
    auto& buf = ed.getBuff();
    const auto& w = ed.getWindow();
    buf.end();
    buf.lineEnd(w.start(), w.dim());
  },
  DEF_HELP() { return "Go to end of the buffer"; });

DEF_CMD(CursorHome, "cursor-home",
        DEF_OP() { ed.getBuff().startOfLine(); },
        DEF_HELP() { return "Go to start of line";});

DEF_CMD(CursorLineEnd, "cursor-line-end",
        DEF_OP() { ed.getBuff().endOfLine(); },
        DEF_HELP() { return "Go to end of line";});

DEF_CMD(
  PageDown, "page-down", DEF_OP() {
    auto& args = ed.getArgs();
    auto& buf = ed.getBuff();
    buf.pageDown(int(args.pageScrollJump * ed.getWindow().dim().y));
  },
  DEF_HELP() {
    return "Scroll down by a page. Definition of a page is as given by the"
      " value of the arg 'pageScrollJump'";
  });

DEF_CMD(
  PageUp, "page-up", DEF_OP() {
    auto& args = ed.getArgs();
    auto& buf = ed.getBuff();
    buf.pageUp(int(args.pageScrollJump * ed.getWindow().dim().y));
  },
  DEF_HELP() {
    return "Scroll up by a page. Definition of a page is as given by the"
      " value of the arg 'pageScrollJump'";
  });

DEF_CMD(NextPara, "next-para",
        DEF_OP() { ed.getBuff().nextPara(); },
        DEF_HELP() { return "Go to start of next paragraph"; });

DEF_CMD(PreviousPara, "previous-para",
        DEF_OP() { ed.getBuff().previousPara(); },
        DEF_HELP() { return "Go to start of previous paragraph"; });

DEF_CMD(NextWord, "next-word",
        DEF_OP() { ed.getBuff().nextWord(); },
        DEF_HELP() { return "Go to start of next word"; });

DEF_CMD(PreviousWord, "previous-word",
        DEF_OP() { ed.getBuff().previousWord(); },
        DEF_HELP() { return "Go to start of previous word"; });

DEF_CMD(
  MatchParen, "match-paren", DEF_OP() {
    auto& buff = ed.getBuff();
    bool isOpen = buff.matchCurrentParen();
    isOpen? buff.lineUp() : buff.lineDown();
  },
  DEF_HELP() { return "Jump to matching parenthesis of current one"; });

DEF_CMD(GotoLine, "goto-line",
        DEF_OP() {
            auto line = ed.prompt("Goto: ");
            int lineNum = str2num(line);
            ed.getBuff().gotoLine(lineNum, ed.getWindow().dim());
        },
        DEF_HELP() { return "Jump to the specified line number"; });

DEF_CMD(AddCursorDown, "add-cursor-down",
        DEF_OP() {
            auto& buf = ed.getBuff();
            auto count = buf.cursorCount();
            auto pos = buf.cursorAt(count - 1);
            if(pos.y >= buf.length()) {
                CMBAR_MSG(ed, "add-cursor-down: reached end of buffer!\n");
                return;
            }
            ++pos.y;
            pos.x = std::min(buf.at(pos.y).length(), pos.x);
            buf.addCursorFromBack(pos);
        },
        DEF_HELP() { return "Adds a cursor at the next line."; });

DEF_CMD(AddCursorUp, "add-cursor-up",
        DEF_OP() {
            auto& buf = ed.getBuff();
            auto pos = buf.cursorAt(0);
            if(pos.y <= 0) {
                CMBAR_MSG(ed, "add-cursor-up: reached start of buffer!\n");
                return;
            }
            --pos.y;
            pos.x = std::min(buf.at(pos.y).length(), pos.x);
            buf.addCursorFromFront(pos);
        },
        DEF_HELP() { return "Adds a cursor at the previous line."; });

} // end namespace CursorOps
} // end namespace teditor
