#include "core/editor.h"
#include "core/command.h"
#include <stdio.h>


namespace teditor {

CMD_NO_UNDO(PromptQuit, "prompt-quit") {
    Editor::getInstance().requestQuitPromptLoop();
}

CMD_NO_UNDO(PromptCancel, "prompt-cancel") {
    auto& ed = Editor::getInstance();
    ed.requestCancelPromptLoop();
    ed.requestQuitPromptLoop();
}

CMD_NO_UNDO(PromptInsertChar, "prompt-insert-char") {
    auto& ed = Editor::getInstance();
    auto& in = ed.getInput();
    auto& cmBar = ed.getCmBar();
    auto c = (char)in.mk.getKey();
    cmBar.insert(c);
}

CMD_NO_UNDO(PromptInsertCharQuit, "prompt-insert-char-quit") {
    auto& ed = Editor::getInstance();
    auto& in = ed.getInput();
    auto& cmBar = ed.getCmBar();
    auto c = (char)in.mk.getKey();
    cmBar.insert(c);
    ed.requestQuitPromptLoop();
}

CMD_NO_UNDO(PromptCursorRight, "prompt-cursor-right") {
    auto& cmBar = Editor::getInstance().getCmBar();
    cmBar.right();
}

CMD_NO_UNDO(PromptCursorLeft, "prompt-cursor-left") {
    auto& cmBar = Editor::getInstance().getCmBar();
    cmBar.left();
}

CMD_NO_UNDO(PromptCursorHome, "prompt-cursor-home") {
    auto& cmBar = Editor::getInstance().getCmBar();
    cmBar.startOfLine();
}

CMD_NO_UNDO(PromptCursorLineEnd, "prompt-cursor-line-end") {
    auto& cmBar = Editor::getInstance().getCmBar();
    cmBar.endOfLine();
}

CMD_NO_UNDO(PromptBackspaceChar, "prompt-backspace-char") {
    auto& cmBar = Editor::getInstance().getCmBar();
    cmBar.removeChar();
    cmBar.updateChoices();
}

CMD_NO_UNDO(PromptDeleteChar, "prompt-delete-char") {
    auto& cmBar = Editor::getInstance().getCmBar();
    cmBar.removeCurrentChar();
    cmBar.updateChoices();
}

CMD_NO_UNDO(PromptOptionsDown, "prompt-options-down") {
    auto& cmBar = Editor::getInstance().getCmBar();
    cmBar.down();
}

CMD_NO_UNDO(PromptOptionsUp, "prompt-options-up") {
    auto& cmBar = Editor::getInstance().getCmBar();
    cmBar.up();
}

} // end namespace teditor
