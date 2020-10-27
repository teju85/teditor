#include "core/editor.h"
#include "core/command.h"
#include "core/option.h"

namespace teditor {
namespace cursor {
namespace ops {

/**
 * @page cursor_ops Operations on cursor
 * All operations that can be applied on a Cursor inside a Buffer.
 *
 * @tableofcontents
 */

/**
 * @page cursor_ops
 * @section cursor-down
 * Moves cursor a line down.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorDown, "cursor-down",
        DEF_OP() { ed.getBuff().down(); },
        DEF_HELP() { return "Move cursor(s) a line down"; });

/**
 * @page cursor_ops
 * @section cursor-up
 * Moves cursor a line up.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorUp, "cursor-up",
        DEF_OP() { ed.getBuff().up(); },
        DEF_HELP() { return "Move cursor(s) a line up"; });

/**
 * @page cursor_ops
 * @section cursor-right
 * Moves cursor a character right.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorRight, "cursor-right",
        DEF_OP() { ed.getBuff().right(); },
        DEF_HELP() { return "Move cursor(s) a char to right"; });

/**
 * @page cursor_ops
 * @section cursor-left
 * Moves cursor a character left.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorLeft, "cursor-left",
        DEF_OP() { ed.getBuff().left(); },
        DEF_HELP() { return "Move cursor(s) a char to left"; });

/**
 * @page cursor_ops
 * @section cursor-reset
 * Go to the start of the Buffer.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorReset, "cursor-reset",
        DEF_OP() { ed.getBuff().begin(); },
        DEF_HELP() { return "Go to start of the buffer"; });

/**
 * @page cursor_ops
 * @section cursor-end
 * Go to the end of the Buffer.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  CursorEnd, "cursor-end", DEF_OP() {
    auto& buf = ed.getBuff();
    const auto& w = ed.getWindow();
    buf.end();
    buf.lineEnd(w.start(), w.dim());
  },
  DEF_HELP() { return "Go to end of the buffer"; });

/**
 * @page cursor_ops
 * @section cursor-home
 * Go to the start of the current Line in the Buffer.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorHome, "cursor-home",
        DEF_OP() { ed.getBuff().startOfLine(); },
        DEF_HELP() { return "Go to start of line";});

/**
 * @page cursor_ops
 * @section cursor-line-end
 * Go to the end of the current Line in the Buffer.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorLineEnd, "cursor-line-end",
        DEF_OP() { ed.getBuff().endOfLine(); },
        DEF_HELP() { return "Go to end of line";});

/**
 * @page cursor_ops
 * @section page-down
 * Scroll down by a page. Definition of a page is as given by the value in the
 * Option `pageScrollJump`.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  PageDown, "page-down", DEF_OP() {
    auto jump = Option::get("pageScrollJump").getReal();
    auto& buf = ed.getBuff();
    buf.pageDown(int(jump * ed.getWindow().dim().y));
  },
  DEF_HELP() {
    return "Scroll down by a page. Definition of a page is as given by the"
      " value of the arg 'pageScrollJump'";
  });

/**
 * @page cursor_ops
 * @section page-up
 * Scroll up by a page. Definition of a page is as given by the value in the
 * Option `pageScrollJump`.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  PageUp, "page-up", DEF_OP() {
    auto jump = Option::get("pageScrollJump").getReal();
    auto& buf = ed.getBuff();
    buf.pageUp(int(jump * ed.getWindow().dim().y));
  },
  DEF_HELP() {
    return "Scroll up by a page. Definition of a page is as given by the"
      " value of the arg 'pageScrollJump'";
  });

/**
 * @page cursor_ops
 * @section next-para
 * Jump to the beginning of the next paragraph.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(NextPara, "next-para",
        DEF_OP() { ed.getBuff().nextPara(); },
        DEF_HELP() { return "Go to start of next paragraph"; });

/**
 * @page cursor_ops
 * @section previous-para
 * Jump to the beginning of the previous paragraph.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(PreviousPara, "previous-para",
        DEF_OP() { ed.getBuff().previousPara(); },
        DEF_HELP() { return "Go to start of previous paragraph"; });

/**
 * @page cursor_ops
 * @section next-word
 * Jump to the beginning of the next word.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(NextWord, "next-word",
        DEF_OP() { ed.getBuff().nextWord(); },
        DEF_HELP() { return "Go to start of next word"; });

/**
 * @page cursor_ops
 * @section previous-word
 * Jump to the beginning of the previous word.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(PreviousWord, "previous-word",
        DEF_OP() { ed.getBuff().previousWord(); },
        DEF_HELP() { return "Go to start of previous word"; });

/**
 * @page cursor_ops
 * @section match-paren
 * Jump to the matching parenthesis of the current one.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  MatchParen, "match-paren", DEF_OP() {
    auto& buff = ed.getBuff();
    auto& win = ed.getWindow();
    bool isOpen = buff.matchCurrentParen();
    isOpen? buff.lineUp(win.dim()) : buff.lineDown();
  },
  DEF_HELP() { return "Jump to matching parenthesis of current one"; });

/**
 * @page cursor_ops
 * @section goto-line
 * Prompts the user for a line number and jumps to the specified line.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  GotoLine, "goto-line", DEF_OP() {
    auto line = ed.prompt("Goto: ");
    int lineNum = str2num(line);
    ed.getBuff().gotoLine(lineNum, ed.getWindow().dim());
  },
  DEF_HELP() { return "Jump to the specified line number"; });

} // end namespace ops
} // end namespace cursor
} // end namespace teditor
