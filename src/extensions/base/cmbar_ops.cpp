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
DEF_CMD(Quit, "prompt-quit",
        DEF_OP() { ed.requestQuitPromptLoop(); },
        DEF_HELP() { return "Quit the current prompt"; });

/**
 * @page cmbar_ops
 * @section prompt-cancel
 * Cancels the currently active prompt. All results/state from the interaction
 * with the current prompt are ignored. This will "move" the cursor from the
 * command prompt into the current Buffer.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  Cancel, "prompt-cancel", DEF_OP() {
    ed.requestCancelPromptLoop();
    ed.requestQuitPromptLoop();
  },
  DEF_HELP() { return "Cancels the currently active prompt"; });

/**
 * @page cmbar_ops
 * @section prompt-insert-char
 * Inserts a character at the current cursor position of the command prompt and
 * moves the cursor one character to the right.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  InsertChar, "prompt-insert-char", DEF_OP() {
    auto& cmBar = ed.getCmBar();
    auto c = (char)ed.getKey();
    cmBar.insert(c);
  },
  DEF_HELP() {
    return "Insert a character at the current cursor position"
      " and move the cursor one character to the right.";
  });

/**
 * @page cmbar_ops
 * @section prompt-insert-char-quit
 * Inserts a character and quits the prompt
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  InsertCharQuit, "prompt-insert-char-quit", DEF_OP() {
    ed.runCmd("prompt-insert-char");
    ed.runCmd("prompt-quit");
  },
  DEF_HELP() { return "Insert character and quit the prompt"; });

/**
 * @page cmbar_ops
 * @section prompt-cursor-right
 * Moves the cursor on the command prompt one char to the right.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorRight, "prompt-cursor-right",
        DEF_OP() { ed.getCmBar().right(); },
        DEF_HELP() { return "Move cursor one char to the right"; });

/**
 * @page cmbar_ops
 * @section prompt-cursor-left
 * Moves the cursor on the command prompt one char to the left.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorLeft, "prompt-cursor-left",
        DEF_OP() { ed.getCmBar().left(); },
        DEF_HELP() { return "Move cursor one char to the left"; });

/**
 * @page cmbar_ops
 * @section prompt-cursor-home
 * Moves the cursor on the command prompt to the beginning of the prompt.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorHome, "prompt-cursor-home",
        DEF_OP() { ed.getCmBar().startOfLine(); },
        DEF_HELP() { return "Move cursor to the beginning of the prompt"; });

/**
 * @page cmbar_ops
 * @section prompt-cursor-line-end
 * Moves the cursor on the command prompt to the end of the prompt.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(CursorLineEnd, "prompt-cursor-line-end",
        DEF_OP() { ed.getCmBar().endOfLine(); },
        DEF_HELP() { return "Move cursor to the end of the prompt"; });

/**
 * @page cmbar_ops
 * @section prompt-backspace-char
 * Removes one character from the left of the cursor on the command prompt and
 * moves the cursor one character to the left.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  BackspaceChar, "prompt-backspace-char", DEF_OP() {
    auto& cmBar = ed.getCmBar();
    cmBar.remove();
    cmBar.updateChoices();
  },
  DEF_HELP() { return "Remove one char from the left of the cursor"; });

/**
 * @page cmbar_ops
 * @section prompt-delete-char
 * Removes the character on the current cursor position. The cursor stays at the
 * current location, unmoved, after the execution of this command.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  DeleteChar, "prompt-delete-char", DEF_OP() {
    auto& cmBar = ed.getCmBar();
    cmBar.remove(true);
    cmBar.updateChoices();
  },
  DEF_HELP() { return "Remove one char at the cursor"; });

/**
 * @page cmbar_ops
 * @section prompt-options-down
 * Scroll down in the options list, in order to select one of them.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(OptionsDown, "prompt-options-down",
        DEF_OP() { ed.getCmBar().down(); },
        DEF_HELP() { return "Scroll down in the options list"; });

/**
 * @page cmbar_ops
 * @section prompt-options-up
 * Scroll up in the options list, in order to select one of them.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(OptionsUp, "prompt-options-up",
        DEF_OP() { ed.getCmBar().up(); },
        DEF_HELP() { return "Scroll up in the options list"; });

} // end namespace ops
} // end namespace cmbar
} // end namespace teditor
