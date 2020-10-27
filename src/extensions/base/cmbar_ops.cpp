#include "core/editor.h"
#include "core/command.h"
#include <stdio.h>


namespace teditor {
namespace cmbar {
namespace ops {

/**
 * @page cmbar_ops Operations on the command status bar
 * All operations that can be applied on the command status that is visible at
 * the bottom of a Buffer.
 *
 * @tableofcontents
 */

/**
 * @page cmbar_ops
 * @section prompt-quit
 * Quit the current prompt. This will "move" the cursor from the command prompt
 * into the current Buffer.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(Quit, "prompt-quit", "cmbar_ops",
        DEF_OP() { ed.requestQuitPromptLoop(); });

/**
 * @page cmbar_ops
 * @section prompt-cancel
 * Cancels the currently active prompt. All results/state from the interaction
 * with the current prompt are ignored. This will "move" the cursor from the
 * command prompt into the current Buffer.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(Cancel, "prompt-cancel", "cmbar_ops", DEF_OP() {
    ed.requestCancelPromptLoop();
    ed.requestQuitPromptLoop();
  });

/**
 * @page cmbar_ops
 * @section prompt-insert-char
 * Inserts a character at the current cursor position of the command prompt and
 * moves the cursor one character to the right.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(InsertChar, "prompt-insert-char", "cmbar_ops", DEF_OP() {
    auto& cmBar = ed.getCmBar();
    auto c = (char)ed.getKey();
    cmBar.insert(c);
  });

/**
 * @page cmbar_ops
 * @section prompt-insert-char-quit
 * Inserts a character and quits the prompt
 *
 * @note Available since v1.0.0
 */
DEF_CMD(InsertCharQuit, "prompt-insert-char-quit", "cmbar_ops", DEF_OP() {
    ed.runCmd("prompt-insert-char");
    ed.runCmd("prompt-quit");
  });

/**
 * @page cmbar_ops
 * @section prompt-cursor-right
 * Moves the cursor on the command prompt one char to the right.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorRight, "prompt-cursor-right", "cmbar_ops",
        DEF_OP() { ed.getCmBar().right(); });

/**
 * @page cmbar_ops
 * @section prompt-cursor-left
 * Moves the cursor on the command prompt one char to the left.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorLeft, "prompt-cursor-left", "cmbar_ops",
        DEF_OP() { ed.getCmBar().left(); });

/**
 * @page cmbar_ops
 * @section prompt-cursor-home
 * Moves the cursor on the command prompt to the beginning of the prompt.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorHome, "prompt-cursor-home", "cmbar_ops",
        DEF_OP() { ed.getCmBar().startOfLine(); });

/**
 * @page cmbar_ops
 * @section prompt-cursor-line-end
 * Moves the cursor on the command prompt to the end of the prompt.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorLineEnd, "prompt-cursor-line-end", "cmbar_ops",
        DEF_OP() { ed.getCmBar().endOfLine(); });

/**
 * @page cmbar_ops
 * @section prompt-backspace-char
 * Removes one character from the left of the cursor on the command prompt and
 * moves the cursor one character to the left.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(BackspaceChar, "prompt-backspace-char", "cmbar_ops", DEF_OP() {
    auto& cmBar = ed.getCmBar();
    cmBar.remove();
    cmBar.updateChoices();
  });

/**
 * @page cmbar_ops
 * @section prompt-delete-char
 * Removes the character on the current cursor position. The cursor stays at the
 * current location, unmoved, after the execution of this command.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(DeleteChar, "prompt-delete-char", "cmbar_ops", DEF_OP() {
    auto& cmBar = ed.getCmBar();
    cmBar.remove(true);
    cmBar.updateChoices();
  });

/**
 * @page cmbar_ops
 * @section prompt-options-down
 * Scroll down in the options list, in order to select one of them.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(OptionsDown, "prompt-options-down", "cmbar_ops",
        DEF_OP() { ed.getCmBar().down(); });

/**
 * @page cmbar_ops
 * @section prompt-options-up
 * Scroll up in the options list, in order to select one of them.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(OptionsUp, "prompt-options-up", "cmbar_ops",
        DEF_OP() { ed.getCmBar().up(); });

} // end namespace ops
} // end namespace cmbar
} // end namespace teditor
