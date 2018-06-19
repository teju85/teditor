#include "editor.h"
#include "command.h"
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
    auto& ed = Editor::getInstance();
    auto& cmBar = ed.getCmBar();
    cmBar.getCursor().right(&cmBar);
}

CMD_NO_UNDO(PromptCursorLeft, "prompt-cursor-left") {
    auto& ed = Editor::getInstance();
    auto& cmBar = ed.getCmBar();
    cmBar.getCursor().left(&cmBar);
}

CMD_NO_UNDO(PromptCursorHome, "prompt-cursor-home") {
    auto& ed = Editor::getInstance();
    auto& cmBar = ed.getCmBar();
    cmBar.getCursor().home(&cmBar);
}

CMD_NO_UNDO(PromptCursorLineEnd, "prompt-cursor-line-end") {
    auto& ed = Editor::getInstance();
    auto& cmBar = ed.getCmBar();
    cmBar.getCursor().lineEnd(&cmBar);
}

CMD_NO_UNDO(PromptBackspaceChar, "prompt-backspace-char") {
    auto& ed = Editor::getInstance();
    auto& cmBar = ed.getCmBar();
    cmBar.remove();
}

CMD_NO_UNDO(PromptDeleteChar, "prompt-delete-char") {
    auto& ed = Editor::getInstance();
    auto& cmBar = ed.getCmBar();
    cmBar.removeCurrent();
}

} // end namespace teditor

