#include "core/editor.h"
#include "core/command.h"
#include "core/option.h"

namespace teditor {
namespace cursor {
namespace ops {

/**
 * @page cursor_ops Cursor
 * All operations that can be applied on a Cursor inside a Buffer.
 *
 * @tableofcontents
 *
 *
 * @section cursor-down
 * Moves cursor a line down.
 *
 * @note Available since v1.0.0
 *
 *
 * @section cursor-up
 * Moves cursor a line up.
 *
 * @note Available since v1.0.0
 *
 *
 * @section cursor-right
 * Moves cursor a character right.
 *
 * @note Available since v1.0.0
 *
 *
 * @section cursor-left
 * Moves cursor a character left.
 *
 * @note Available since v1.0.0
 *
 *
 * @section cursor-reset
 * Go to the start of the Buffer.
 *
 * @note Available since v1.0.0
 *
 *
 * @section cursor-end
 * Go to the end of the Buffer.
 *
 * @note Available since v1.0.0
 *
 *
 * @section cursor-home
 * Go to the start of the current Line in the Buffer.
 *
 * @note Available since v1.0.0
 *
 *
 * @section cursor-line-end
 * Go to the end of the current Line in the Buffer.
 *
 * @note Available since v1.0.0
 *
 *
 * @section page-down
 * Scroll down by a page. Definition of a page is as given by the value in the
 * Option `pageScrollJump`.
 *
 * @note Available since v1.0.0
 *
 *
 * @section page-up
 * Scroll up by a page. Definition of a page is as given by the value in the
 * Option `pageScrollJump`.
 *
 * @note Available since v1.0.0
 *
 *
 * @section next-para
 * Jump to the beginning of the next paragraph.
 *
 * @note Available since v1.0.0
 *
 *
 * @section previous-para
 * Jump to the beginning of the previous paragraph.
 *
 * @note Available since v1.0.0
 *
 *
 * @section next-word
 * Jump to the beginning of the next word.
 *
 * @note Available since v1.0.0
 *
 *
 * @section previous-word
 * Jump to the beginning of the previous word.
 *
 * @note Available since v1.0.0
 *
 *
 * @addtogroup cursor_ops_group
 * @section match-paren
 * Jump to the matching parenthesis of the current one.
 *
 * @note Available since v1.0.0
 *
 *
 * @section goto-line
 * Prompts the user for a line number and jumps to the specified line.
 *
 * @note Available since v1.0.0
 */

DEF_CMD(CursorDown, "cursor-down", "cursor_ops",
        DEF_OP() { ed.getBuff().down(); });

DEF_CMD(CursorUp, "cursor-up", "cursor_ops",
        DEF_OP() { ed.getBuff().up(); });

DEF_CMD(CursorRight, "cursor-right", "cursor_ops",
        DEF_OP() { ed.getBuff().right(); });

DEF_CMD(CursorLeft, "cursor-left", "cursor_ops",
        DEF_OP() { ed.getBuff().left(); });

DEF_CMD(CursorReset, "cursor-reset", "cursor_ops",
        DEF_OP() { ed.getBuff().begin(); });

DEF_CMD(CursorEnd, "cursor-end", "cursor_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    const auto& w = ed.getWindow();
    buf.end();
    buf.lineEnd(w.start(), w.dim());
  });

DEF_CMD(CursorHome, "cursor-home", "cursor_ops",
        DEF_OP() { ed.getBuff().startOfLine(); });

DEF_CMD(CursorLineEnd, "cursor-line-end", "cursor_ops",
        DEF_OP() { ed.getBuff().endOfLine(); });

DEF_CMD(PageDown, "page-down", "cursor_ops", DEF_OP() {
    auto jump = Option::get("pageScrollJump").getReal();
    auto& buf = ed.getBuff();
    buf.pageDown(int(jump * ed.getWindow().dim().y));
  });

DEF_CMD(PageUp, "page-up", "cursor_ops", DEF_OP() {
    auto jump = Option::get("pageScrollJump").getReal();
    auto& buf = ed.getBuff();
    buf.pageUp(int(jump * ed.getWindow().dim().y));
  });

DEF_CMD(NextPara, "next-para", "cursor_ops",
        DEF_OP() { ed.getBuff().nextPara(); });

DEF_CMD(PreviousPara, "previous-para", "cursor_ops",
        DEF_OP() { ed.getBuff().previousPara(); });

DEF_CMD(NextWord, "next-word", "cursor_ops",
        DEF_OP() { ed.getBuff().nextWord(); });

DEF_CMD(PreviousWord, "previous-word", "cursor_ops",
        DEF_OP() { ed.getBuff().previousWord(); });

DEF_CMD(MatchParen, "match-paren", "cursor_ops", DEF_OP() {
    auto& buff = ed.getBuff();
    auto& win = ed.getWindow();
    bool isOpen = buff.matchCurrentParen();
    isOpen? buff.lineUp(win.dim()) : buff.lineDown();
  });

DEF_CMD(GotoLine, "goto-line", "cursor_ops", DEF_OP() {
    auto line = ed.prompt("Goto: ");
    int lineNum = str2num(line);
    ed.getBuff().gotoLine(lineNum, ed.getWindow().dim());
  });

} // end namespace ops
} // end namespace cursor
} // end namespace teditor
