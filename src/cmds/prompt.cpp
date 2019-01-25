#include "core/editor.h"
#include "core/command.h"
#include <stdio.h>


namespace teditor {
namespace Prompt {

DEF_CMD(Quit, "prompt-quit",
        DEF_OP() { ed.requestQuitPromptLoop(); },
        DEF_HELP() { return "Quit the current prompt"; });

DEF_CMD(Cancel, "prompt-cancel",
        DEF_OP() {
            ed.requestCancelPromptLoop();
            ed.requestQuitPromptLoop();
        },
        DEF_HELP() { return "Cancels the currently active prompt"; });

DEF_CMD(InsertChar, "prompt-insert-char",
        DEF_OP() {
            auto& in = ed.getInput();
            auto& cmBar = ed.getCmBar();
            auto c = (char)in.mk.getKey();
            cmBar.insert(c);
        },
        DEF_HELP() {
            return "Insert a character at the current cursor position"
                " and move the cursor one character to the right.";
        });

DEF_CMD(InsertCharQuit, "prompt-insert-char-quit",
        DEF_OP() {
            ed.runCmd("prompt-insert-char");
            ed.runCmd("prompt-quit");
        },
        DEF_HELP() { return "Insert character and quit the prompt"; });

DEF_CMD(CursorRight, "prompt-cursor-right",
        DEF_OP() { ed.getCmBar().right(); },
        DEF_HELP() { return "Move cursor one char to the right"; });

DEF_CMD(CursorLeft, "prompt-cursor-left",
        DEF_OP() { ed.getCmBar().left(); },
        DEF_HELP() { return "Move cursor one char to the left"; });

DEF_CMD(CursorHome, "prompt-cursor-home",
        DEF_OP() { ed.getCmBar().startOfLine(); },
        DEF_HELP() { return "Move cursor to the beginning of the prompt"; });

DEF_CMD(CursorLineEnd, "prompt-cursor-line-end",
        DEF_OP() { ed.getCmBar().endOfLine(); },
        DEF_HELP() { return "Move cursor to the end of the prompt"; });

DEF_CMD(BackspaceChar, "prompt-backspace-char",
        DEF_OP() {
            auto& cmBar = ed.getCmBar();
            cmBar.remove();
            cmBar.updateChoices();
        },
        DEF_HELP() { return "Remove one char from the left of the cursor"; });

DEF_CMD(DeleteChar, "prompt-delete-char",
        DEF_OP() {
            auto& cmBar = ed.getCmBar();
            cmBar.remove(true);
            cmBar.updateChoices();
        },
        DEF_HELP() { return "Remove one char at the cursor"; });

DEF_CMD(OptionsDown, "prompt-options-down",
        DEF_OP() { ed.getCmBar().down(); },
        DEF_HELP() { return "Scroll down in the options list"; });

DEF_CMD(OptionsUp, "prompt-options-up",
        DEF_OP() { ed.getCmBar().up(); },
        DEF_HELP() { return "Scroll up in the options list"; });

} // end namespace Prompt
} // end namespace teditor
