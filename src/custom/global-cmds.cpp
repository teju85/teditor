#include "editor.h"
#include "command.h"
#include <stdio.h>


namespace teditor {

CMD_NO_UNDO(Quit, "quit") {
    auto& ed = Editor::getInstance();
    ed.requestQuitEventLoop();
    ed.checkForModifiedBuffers();
}

///@todo: support for when before is before regs!
///@todo: there's still a newline issue when selection spans the whole last line
/// v/s selection ends at the beginning of the last line!
CMD_UNDO3(RemoveRegion, "remove-region", Positions regs, Positions before,
          std::vector<std::string> del) {
    auto& ed = Editor::getInstance();
    if(type == CMD_FRESH && !ed.isRegionActive()) {
        canIundo = false;
        return;
    }
    auto& mlbuffer = ed.getBuff();
    auto& cu = mlbuffer.getCursor();
    switch(type) {
    case CMD_UNDO:
        cu.restoreExcursion(regs);
        mlbuffer.insert(del);
        ed.stopRegion();
        break;
    case CMD_REDO:
        del = mlbuffer.remove(regs, before);
        cu.restoreExcursion(regs);
        ed.stopRegion();
        break;
    default:
        before = cu.saveExcursion();
        regs = mlbuffer.getRegionLocs();
        del = mlbuffer.remove(regs, before);
        cu.restoreExcursion(regs);
        ed.stopRegion();
    };
}

CMD_UNDO2(InsertChar, "insert-char", char c, Positions before) {
    auto& ed = Editor::getInstance();
    auto& mlbuffer = ed.getBuff();
    auto& cu = mlbuffer.getCursor();
    if(type == CMD_FRESH && ed.isRegionActive())
        ed.runCmd("remove-region");
    switch(type) {
    case CMD_UNDO:
        cu.restoreExcursion(before);
        mlbuffer.removeCurrent();
        break;
    case CMD_REDO:
        cu.restoreExcursion(before);
        mlbuffer.insert(c);
        break;
    default:
        before = cu.saveExcursion();
        auto& in = ed.getInput();
        c = (char)in.mk.getKey();
        mlbuffer.insert(c);
    };
}

CMD_UNDO3(BackspaceChar, "backspace-char", std::vector<std::string> del,
          Positions before, Positions after) {
    auto& ed = Editor::getInstance();
    if(type == CMD_FRESH && ed.isRegionActive()) {
        canIundo = false;
        ed.runCmd("remove-region");
        return;
    }
    auto& mlbuffer = ed.getBuff();
    auto& cu = mlbuffer.getCursor();
    switch(type) {
    case CMD_UNDO:
        cu.restoreExcursion(after);
        mlbuffer.insert(del);
        break;
    case CMD_REDO:
        cu.restoreExcursion(before);
        mlbuffer.remove();
        break;
    default:
        before = cu.saveExcursion();
        del = mlbuffer.remove();
        after = cu.saveExcursion();
    };
}

CMD_UNDO2(DeleteChar, "delete-char", std::vector<std::string> del,
          Positions locs) {
    auto& ed = Editor::getInstance();
    if(type == CMD_FRESH && ed.isRegionActive()) {
        canIundo = false;
        ed.runCmd("remove-region");
        return;
    }
    auto& mlbuffer = ed.getBuff();
    auto& cu = mlbuffer.getCursor();
    switch(type) {
    case CMD_UNDO:
        cu.restoreExcursion(locs);
        mlbuffer.insert(del);
        cu.restoreExcursion(locs);
        break;
    case CMD_REDO:
        cu.restoreExcursion(locs);
        mlbuffer.removeCurrent();
        break;
    default:
        locs = cu.saveExcursion();
        del = mlbuffer.removeCurrent();
    };
}

CMD_UNDO2(KillLine, "kill-line", std::vector<std::string> del,
          Positions locs) {
    auto& ed = Editor::getInstance();
    auto& buf = ed.getBuff();
    auto& cu = buf.getCursor();
    if(ed.isRegionActive())
        ed.stopRegion();
    switch(type) {
    case CMD_UNDO:
        cu.restoreExcursion(locs);
        buf.insert(del);
        cu.restoreExcursion(locs);
        break;
    case CMD_REDO:
        cu.restoreExcursion(locs);
        buf.killLine();
        break;
    default:
        locs = cu.saveExcursion();
        del = buf.killLine();
    };
}

CMD_UNDO2(KeepLines, "keep-lines", RemovedLines lines, std::string regex) {
    auto& ed = Editor::getInstance();
    auto& buf = ed.getBuff();
    auto& cu = buf.getCursor();
    if(cu.count() > 1) {
        CMBAR_MSG("keep-lines works only with single cursor!\n");
        return;
    }
    switch(type) {
    case CMD_UNDO:
        buf.addLines(lines);
        break;
    case CMD_REDO: {
        Pcre pc(regex, true);
        buf.keepRemoveLines(pc, true);
        break;
    }
    default:
        regex = ed.prompt("Regex: ");
        Pcre pc(regex, true);
        lines = buf.keepRemoveLines(pc, true);
    };
}

CMD_UNDO2(RemoveLines, "remove-lines", RemovedLines lines, std::string regex) {
    auto& ed = Editor::getInstance();
    auto& buf = ed.getBuff();
    auto& cu = buf.getCursor();
    if(cu.count() > 1) {
        CMBAR_MSG("remove-lines works only with single cursor!\n");
        return;
    }
    switch(type) {
    case CMD_UNDO:
        buf.addLines(lines);
        break;
    case CMD_REDO: {
        Pcre pc(regex, true);
        buf.keepRemoveLines(pc, false);
        break;
    }
    default:
        regex = ed.prompt("Regex: ");
        Pcre pc(regex, true);
        lines = buf.keepRemoveLines(pc, false);
    };
}

CMD_UNDO3(ShellToBuffer, "shell-to-buffer", Positions before, Positions after,
          std::string output) {
    auto& ed = Editor::getInstance();
    if(ed.isRegionActive())
        ed.stopRegion();
    auto& mlb = ed.getBuff();
    auto& cu = mlb.getCursor();
    switch(type) {
    case CMD_UNDO:
        cu.restoreExcursion(before);
        mlb.remove(before, after);
        break;
    case CMD_REDO:
        cu.restoreExcursion(before);
        mlb.insert(output.c_str());
        break;
    default:
        auto cmd = ed.prompt("Shell Command: ");
        if(cmd.empty()) {
            canIundo = false;
            return;
        }
        auto res = check_output(cmd);
        output = res.output;
        if(output.empty()) {
            canIundo = false;
            return;
        }
        before = cu.saveExcursion();
        mlb.insert(output.c_str());
        after = cu.saveExcursion();
    };
}

CMD_UNDO3(PasteRegion, "paste-region", std::vector<std::string> del,
          Positions before, Positions after) {
    auto& ed = Editor::getInstance();
    auto& mlbuffer = ed.getBuff();
    auto& cu = mlbuffer.getCursor();
    if(type == CMD_FRESH) {
        if(!ed.hasCopy()) {
            canIundo = false;
            CMBAR_MSG("No selection to paste!\n");
            return;
        }
        if(ed.isRegionActive())
            ed.runCmd("remove-region");
    }
    switch(type) {
    case CMD_UNDO:
        cu.restoreExcursion(before);
        mlbuffer.remove(before, after);
        break;
    case CMD_REDO:
        cu.restoreExcursion(before);
        mlbuffer.insert(del);
        break;
    default:
        before = cu.saveExcursion();
        del = ed.copyData();
        mlbuffer.insert(del);
        after = cu.saveExcursion();
    };
}

///@todo: what if before is before regs!?
CMD_UNDO3(CutRegion, "cut-region", std::vector<std::string> del,
          Positions before, Positions regs) {
    auto& ed = Editor::getInstance();
    auto& mlbuffer = ed.getBuff();
    auto& cu = mlbuffer.getCursor();
    if(type == CMD_FRESH && !ed.isRegionActive()) {
        canIundo = false;
        CMBAR_MSG("No selection to cut!\n");
        return;
    }
    switch(type) {
    case CMD_UNDO:
        cu.restoreExcursion(regs);
        mlbuffer.insert(del);
        ed.stopRegion();
        break;
    case CMD_REDO:
        del = mlbuffer.remove(regs, before);
        cu.restoreExcursion(regs);
        ed.stopRegion();
        break;
    default:
        before = cu.saveExcursion();
        regs = mlbuffer.getRegionLocs();
        del = mlbuffer.remove(regs, before);
        ed.setCopyData(del);
        cu.restoreExcursion(regs);
        ed.stopRegion();
    };
}

CMD_NO_UNDO(CopyRegion, "copy-region") {
    auto& ed = Editor::getInstance();
    if(!ed.isRegionActive()) {
        CMBAR_MSG("No selection to copy!\n");
        return;
    }
    auto& mlb = ed.getBuff();
    auto cp = mlb.regionAsStr();
    ed.setCopyData(cp);
}

CMD_NO_UNDO(ReloadBuffer, "reload-buffer") {
    auto& ed = Editor::getInstance();
    auto& buf = ed.getBuff();
    if(buf.isModified() && ed.promptYesNo("Buffer modified, still reload? "))
        buf.reload();
}

CMD_NO_UNDO(CursorDown, "cursor-down") {
    auto& mlbuffer = Editor::getInstance().getBuff();
    mlbuffer.getCursor().down(&mlbuffer);
}

CMD_NO_UNDO(CursorUp, "cursor-up") {
    auto& mlbuffer = Editor::getInstance().getBuff();
    mlbuffer.getCursor().up(&mlbuffer);
}

CMD_NO_UNDO(CursorRight, "cursor-right") {
    auto& mlbuffer = Editor::getInstance().getBuff();
    mlbuffer.getCursor().right(&mlbuffer);
}

CMD_NO_UNDO(CursorLeft, "cursor-left") {
    auto& mlbuffer = Editor::getInstance().getBuff();
    mlbuffer.getCursor().left(&mlbuffer);
}

CMD_NO_UNDO(CursorReset, "cursor-reset") {
    auto& mlbuffer = Editor::getInstance().getBuff();
    mlbuffer.getCursor().reset(&mlbuffer);
}

CMD_NO_UNDO(CursorEnd, "cursor-end") {
    auto& mlbuffer = Editor::getInstance().getBuff();
    mlbuffer.getCursor().end(&mlbuffer);
}

CMD_NO_UNDO(CursorHome, "cursor-home") {
    auto& mlbuffer = Editor::getInstance().getBuff();
    mlbuffer.getCursor().home(&mlbuffer);
}

CMD_NO_UNDO(CursorLineEnd, "cursor-line-end") {
    auto& mlbuffer = Editor::getInstance().getBuff();
    mlbuffer.getCursor().lineEnd(&mlbuffer);
}

CMD_NO_UNDO(PageDown, "page-down") {
    auto& ed = Editor::getInstance();
    auto& args = ed.getArgs();
    auto& mlbuffer = ed.getBuff();
    mlbuffer.getCursor().pageDown(&mlbuffer, args.pageScrollJump);
}

CMD_NO_UNDO(PageUp, "page-up") {
    auto& ed = Editor::getInstance();
    auto& args = ed.getArgs();
    auto& mlbuffer = ed.getBuff();
    mlbuffer.getCursor().pageUp(&mlbuffer, args.pageScrollJump);
}

CMD_NO_UNDO(NextPara, "next-para") {
    auto& mlbuffer = Editor::getInstance().getBuff();
    mlbuffer.getCursor().nextPara(&mlbuffer);
}

CMD_NO_UNDO(PreviousPara, "previous-para") {
    auto& mlbuffer = Editor::getInstance().getBuff();
    mlbuffer.getCursor().previousPara(&mlbuffer);
}

CMD_NO_UNDO(NextWord, "next-word") {
    auto& mlbuffer = Editor::getInstance().getBuff();
    mlbuffer.getCursor().nextWord(&mlbuffer);
}

CMD_NO_UNDO(PreviousWord, "previous-word") {
    auto& mlbuffer = Editor::getInstance().getBuff();
    mlbuffer.getCursor().previousWord(&mlbuffer);
}

CMD_NO_UNDO(MatchParen, "match-paren") {
    auto& mlbuffer = Editor::getInstance().getBuff();
    mlbuffer.matchCurrentParen();
}

CMD_NO_UNDO(GotoLine, "goto-line") {
    auto& ed = Editor::getInstance();
    auto line = ed.prompt("Goto: ");
    int lineNum = str2num(line);
    ed.getBuff().gotoLine(lineNum);
}

CMD_NO_UNDO(BufferSave, "buffer-save") {
    auto& mlbuffer = Editor::getInstance().getBuff();
    if(mlbuffer.isRO()) {
        CMBAR_MSG("buffer-save: Read Only Buffer!\n");
        return;
    }
    mlbuffer.save();
}

CMD_NO_UNDO(BufferPwd, "pwd") {
    auto& buf = Editor::getInstance().getBuff();
    CMBAR_MSG("Pwd: %s\n", buf.pwd().c_str());
}

CMD_NO_UNDO(BufferNext, "buffer-next") {
    auto& ed = Editor::getInstance();
    ed.incrementCurrBuff();
}

CMD_NO_UNDO(BufferPrev, "buffer-prev") {
    auto& ed = Editor::getInstance();
    ed.decrementCurrBuff();
}

CMD_NO_UNDO(KillCurrBuff, "kill-this-buffer") {
    auto& ed = Editor::getInstance();
    ed.killCurrBuff();
}

CMD_NO_UNDO(KillOtherBuffs, "kill-other-buffers") {
    auto& ed = Editor::getInstance();
    ed.killOtherBuffs();
}

CMD_NO_UNDO(StartRegion, "start-region") {
    auto& ed = Editor::getInstance();
    if(!ed.isRegionActive()) {
        ed.startRegion();
    } else {
        ed.stopRegion();
        ed.startRegion();
    }
}

CMD_NO_UNDO(Cancel, "cancel") {
    auto& ed = Editor::getInstance();
    auto& cu = ed.getBuff().getCursor();
    if(ed.isRegionActive())
        ed.stopRegion();
    if(cu.count() > 1)
        cu.clearAllButFirst();
}

CMD_NO_UNDO(ScratchBuffer, "scratch-buffer") {
    auto& ed = Editor::getInstance();
    ed.createScratchBuff(true);
}

CMD_NO_UNDO(FindFile, "find-file") {
    auto& ed = Editor::getInstance();
    auto file = ed.prompt("Find File: ");
    if(!file.empty())
        ed.load(file, 0);
}

CMD_NO_UNDO(RunCommand, "run-command") {
    auto& ed = Editor::getInstance();
    auto cmd = ed.prompt("Cmd: ");
    try {
        ed.runCmd(cmd);
    } catch(const std::runtime_error& e) {
        CMBAR_MSG("Unknown command: %s!\n", cmd.c_str());
    }
}

CMD_NO_UNDO(ShellCommand, "shell-command") {
    auto& ed = Editor::getInstance();
    auto cmd = ed.prompt("Shell Command: ");
    if(!cmd.empty()) {
        auto res = check_output(cmd);
        MESSAGE("Shell Command: %s (exit-status=%d)\nOutput: %s",
                cmd.c_str(), res.status, res.output.c_str());
    }
}

CMD_NO_UNDO(LaunchExplorer, "open-explorer") {
    auto& ed = Editor::getInstance();
    auto& mlbuffer = ed.getBuff();
    check_output("cygstart " + mlbuffer.pwd());
}

CMD_NO_UNDO(LaunchFirefox, "firefox") {
    auto& ed = Editor::getInstance();
    auto& buf = ed.getBuff();
    std::string url;
    if(ed.isRegionActive())
        url = buf.regionAsStr()[0];
    check_output("cygstart firefox '" + url + "'");
}

CMD_NO_UNDO(LaunchFirefoxPrivate, "fir") {
    auto& ed = Editor::getInstance();
    auto& buf = ed.getBuff();
    std::string url;
    if(ed.isRegionActive())
        url = buf.regionAsStr()[0];
    check_output("cygstart firefox -private-window '" + url + "'");
}

CMD_NO_UNDO(BrowserSearch, "browser-search") {
    OptionMap opts;
    opts["duckduckgo"] = "http://www.duckduckgo.com/?t=hb&ia=meanings&q=\"%s\"";
    opts["google"] = "http://www.google.com/#q=\"%s\"";
    opts["maps"] = "https://www.google.co.in/maps/search/\"%s\"";
    opts["nvsearch"] = "https://nvsearch.nvidia.com/Pages/results.aspx?k=\"%s\"";
    opts["stock"] = "https://duckduckgo.com/?q=\"%s\"&t=ffab&ia=stock";
    opts["youtube"] = "https://www.youtube.com/results?search_query=\"%s\"";
    auto& ed = Editor::getInstance();
    auto command = ed.promptForEnum("Search:", opts);
    if(command.empty())
        return;
    command = "cygstart firefox -private-window '" + command + "'";
    // we'll only look at first cursor!
    auto query = ed.getBuff().regionAsStr();
    std::string queryStr;
    if(query.empty())
        queryStr = ed.prompt("Query: ");
    else
        queryStr = query[0];
    if(queryStr.empty())
        return;
    auto buf = format(command.c_str(), queryStr.c_str());
    check_output(buf.c_str());
}

CMD_NO_UNDO(CommandUndo, "command-undo") {
    auto& buf = Editor::getInstance().getBuff();
    buf.undo();
}

CMD_NO_UNDO(CommandRedo, "command-redo") {
    auto& buf = Editor::getInstance().getBuff();
    buf.redo();
}

CMD_NO_UNDO(AddCursorDown, "add-cursor-down") {
    auto& buf = Editor::getInstance().getBuff();
    auto& cu = buf.getCursor();
    auto count = cu.count();
    auto pos = cu.at(count-1);
    if(pos.y >= buf.length()) {
        CMBAR_MSG("add-cursor-down: reached end of buffer!\n");
        return;
    }
    ++pos.y;
    pos.x = std::min(buf.at(pos.y).length(), pos.x);
    cu.addBack(pos);
}

CMD_NO_UNDO(AddCursorUp, "add-cursor-up") {
    auto& buf = Editor::getInstance().getBuff();
    auto& cu = buf.getCursor();
    auto pos = cu.at(0);
    if(pos.y <= 0) {
        CMBAR_MSG("add-cursor-up: reached start of buffer!\n");
        return;
    }
    --pos.y;
    pos.x = std::min(buf.at(pos.y).length(), pos.x);
    cu.addFront(pos);
}

CMD_NO_UNDO(Download, "download-url") {
    auto& ed = Editor::getInstance();
    auto url = ed.prompt("URL: ");
    if(url.empty())
        return;
    auto file = ed.prompt("File: ");
    if(file.empty())
        return;
    downloadUrl(url, file);
}

CMD_UNDO3(DownloadBuffer, "download-to-buffer", Positions before, Positions after,
          std::string output) {
    auto& ed = Editor::getInstance();
    if(ed.isRegionActive())
        ed.stopRegion();
    auto& mlb = ed.getBuff();
    auto& cu = mlb.getCursor();
    switch(type) {
    case CMD_UNDO:
        cu.restoreExcursion(before);
        mlb.remove(before, after);
        break;
    case CMD_REDO:
        cu.restoreExcursion(before);
        mlb.insert(output.c_str());
        break;
    default:
        auto url = ed.prompt("URL: ");
        if(url.empty()) {
            canIundo = false;
            return;
        }
        auto output = downloadUrlToString(url);
        if(output.empty()) {
            canIundo = false;
            return;
        }
        before = cu.saveExcursion();
        mlb.insert(output.c_str());
        after = cu.saveExcursion();
    };
}

} // end namespace teditor
