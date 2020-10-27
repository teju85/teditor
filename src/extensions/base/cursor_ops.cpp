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
DEF_CMD(CursorDown, "cursor-down", "cursor_ops",
        DEF_OP() { ed.getBuff().down(); });

/**
 * @page cursor_ops
 * @section cursor-up
 * Moves cursor a line up.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorUp, "cursor-up", "cursor_ops",
        DEF_OP() { ed.getBuff().up(); });

/**
 * @page cursor_ops
 * @section cursor-right
 * Moves cursor a character right.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorRight, "cursor-right", "cursor_ops",
        DEF_OP() { ed.getBuff().right(); });

/**
 * @page cursor_ops
 * @section cursor-left
 * Moves cursor a character left.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorLeft, "cursor-left", "cursor_ops",
        DEF_OP() { ed.getBuff().left(); });

/**
 * @page cursor_ops
 * @section cursor-reset
 * Go to the start of the Buffer.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorReset, "cursor-reset", "cursor_ops",
        DEF_OP() { ed.getBuff().begin(); });

/**
 * @page cursor_ops
 * @section cursor-end
 * Go to the end of the Buffer.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorEnd, "cursor-end", "cursor_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    const auto& w = ed.getWindow();
    buf.end();
    buf.lineEnd(w.start(), w.dim());
  });

/**
 * @page cursor_ops
 * @section cursor-home
 * Go to the start of the current Line in the Buffer.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorHome, "cursor-home", "cursor_ops",
        DEF_OP() { ed.getBuff().startOfLine(); });

/**
 * @page cursor_ops
 * @section cursor-line-end
 * Go to the end of the current Line in the Buffer.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorLineEnd, "cursor-line-end", "cursor_ops",
        DEF_OP() { ed.getBuff().endOfLine(); });

/**
 * @page cursor_ops
 * @section page-down
 * Scroll down by a page. Definition of a page is as given by the value in the
 * Option `pageScrollJump`.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(PageDown, "page-down", "cursor_ops", DEF_OP() {
    auto jump = Option::get("pageScrollJump").getReal();
    auto& buf = ed.getBuff();
    buf.pageDown(int(jump * ed.getWindow().dim().y));
  });

/**
 * @page cursor_ops
 * @section page-up
 * Scroll up by a page. Definition of a page is as given by the value in the
 * Option `pageScrollJump`.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(PageUp, "page-up", "cursor_ops", DEF_OP() {
    auto jump = Option::get("pageScrollJump").getReal();
    auto& buf = ed.getBuff();
    buf.pageUp(int(jump * ed.getWindow().dim().y));
  });

/**
 * @page cursor_ops
 * @section next-para
 * Jump to the beginning of the next paragraph.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(NextPara, "next-para", "cursor_ops",
        DEF_OP() { ed.getBuff().nextPara(); });

/**
 * @page cursor_ops
 * @section previous-para
 * Jump to the beginning of the previous paragraph.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(PreviousPara, "previous-para", "cursor_ops",
        DEF_OP() { ed.getBuff().previousPara(); });

/**
 * @page cursor_ops
 * @section next-word
 * Jump to the beginning of the next word.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(NextWord, "next-word", "cursor_ops",
        DEF_OP() { ed.getBuff().nextWord(); });

/**
 * @page cursor_ops
 * @section previous-word
 * Jump to the beginning of the previous word.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(PreviousWord, "previous-word", "cursor_ops",
        DEF_OP() { ed.getBuff().previousWord(); });

/**
 * @page cursor_ops
 * @section match-paren
 * Jump to the matching parenthesis of the current one.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(MatchParen, "match-paren", "cursor_ops", DEF_OP() {
    auto& buff = ed.getBuff();
    auto& win = ed.getWindow();
    bool isOpen = buff.matchCurrentParen();
    isOpen? buff.lineUp(win.dim()) : buff.lineDown();
  });

/**
 * @page cursor_ops
 * @section goto-line
 * Prompts the user for a line number and jumps to the specified line.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(GotoLine, "goto-line", "cursor_ops", DEF_OP() {
    auto line = ed.prompt("Goto: ");
    int lineNum = str2num(line);
    ed.getBuff().gotoLine(lineNum, ed.getWindow().dim());
  });

} // end namespace ops
} // end namespace cursor
} // end namespace teditor
