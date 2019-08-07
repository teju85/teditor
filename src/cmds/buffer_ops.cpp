#include "core/editor.h"
#include "core/command.h"
#include "core/isearch.h"


namespace teditor {
namespace BufferOps {

DEF_CMD(CommandUndo, "command-undo",
        DEF_OP() {
            if(!ed.getBuff().undo())
                CMBAR_MSG(ed, "No further undo information\n");
        },
        DEF_HELP() { return "Undo the previous edition in this buffer."; });

DEF_CMD(CommandRedo, "command-redo",
        DEF_OP() {
            if(!ed.getBuff().redo())
                CMBAR_MSG(ed, "No further redo information\n");
        },
        DEF_HELP() { return "Redo the previous undo in this buffer."; });

DEF_CMD(InsertChar, "insert-char",
        DEF_OP() {
            auto& buf = ed.getBuff();
            if(buf.isRegionActive()) ed.runCmd("backspace-char");
            auto c = (char)ed.getKey();
            buf.insert(c);
        },
        DEF_HELP() { return "Inserts currently pressed char into buffer."; });

DEF_CMD(BackspaceChar, "backspace-char",
        DEF_OP() { ed.getBuff().remove(); },
        DEF_HELP() {
            return "Removes the char to the left of cursor, or if a region is"
                " active, will remove it instead.";
        });

DEF_CMD(DeleteChar, "delete-char",
        DEF_OP() { ed.getBuff().remove(true); },
        DEF_HELP() {
            return "Removes the char on the cursor, or if a region is active,"
                " will remove it instead";
        });

// CMD_UNDO4(SortLines, "sort-lines", Positions after, Positions regs,
//           Strings del, Positions before) {
//     if(type == CMD_FRESH && !buf.isRegionActive()) {
//         canIundo = false;
//         return;
//     }
//     auto& buf = ed.getBuff();
//     switch(type) {
//     case CMD_UNDO:
//         buf.removeRegion(regs, after);
//         buf.restoreCursors(regs);
//         buf.insert(del);
//         break;
//     case CMD_REDO:
//         buf.restoreCursors(regs);
//         buf.startRegion();
//         buf.restoreCursors(before);
//         buf.sortRegions();
//         break;
//     default:
//         regs = buf.getRegionLocs();
//         del = buf.regionAsStr();
//         before = buf.saveCursors();
//         buf.sortRegions();
//         after = buf.saveCursors();
//     };
//     buf.stopRegion();
// }

// CMD_UNDO2(KillLine, "kill-line", Strings del, Positions locs) {
//     auto& buf = ed.getBuff();
//     if(buf.isRegionActive())
//         buf.stopRegion();
//     switch(type) {
//     case CMD_UNDO:
//         buf.restoreCursors(locs);
//         buf.insert(del);
//         buf.restoreCursors(locs);
//         break;
//     case CMD_REDO:
//         buf.restoreCursors(locs);
//         buf.killLine();
//         break;
//     default:
//         locs = buf.saveCursors();
//         del = buf.killLine();
//     };
// }

// CMD_UNDO2(KeepLines, "keep-lines", RemovedLines lines, std::string regex) {
//     auto& buf = ed.getBuff();
//     if(buf.cursorCount() > 1) {
//         CMBAR_MSG(ed, "keep-lines works only with single cursor!\n");
//         return;
//     }
//     switch(type) {
//     case CMD_UNDO:
//         buf.addLines(lines);
//         break;
//     case CMD_REDO: {
//         Pcre pc(regex, true);
//         buf.keepRemoveLines(pc, true);
//         break;
//     }
//     default:
//         regex = ed.prompt("Regex: ");
//         Pcre pc(regex, true);
//         lines = buf.keepRemoveLines(pc, true);
//     };
// }

// CMD_UNDO2(RemoveLines, "remove-lines", RemovedLines lines, std::string regex) {
//     auto& buf = ed.getBuff();
//     if(buf.cursorCount() > 1) {
//         CMBAR_MSG(ed, "remove-lines works only with single cursor!\n");
//         return;
//     }
//     switch(type) {
//     case CMD_UNDO:
//         buf.addLines(lines);
//         break;
//     case CMD_REDO: {
//         Pcre pc(regex, true);
//         buf.keepRemoveLines(pc, false);
//         break;
//     }
//     default:
//         regex = ed.prompt("Regex: ");
//         Pcre pc(regex, true);
//         lines = buf.keepRemoveLines(pc, false);
//     };
// }

DEF_CMD(ShellToBuffer, "shell-to-buffer",
        DEF_OP() {
            auto& buf = ed.getBuff();
            if(buf.isRegionActive()) buf.remove();
            auto cmd = ed.prompt("Shell Command: ");
            if(cmd.empty()) return;
            auto res = check_output(cmd);
            if(res.output.empty()) return;
            buf.insert(res.output);
        },
        DEF_HELP() {
            return "Prompts the user for a command, executes it inside a shall"
                " and inserts its output into the current buffer.";
        });

DEF_CMD(PasteRegion, "paste-region",
        DEF_OP() {
            auto& buf = ed.getBuff();
            if(!ed.hasCopy()) {
                CMBAR_MSG(ed, "No selection to paste!\n");
                return;
            }
            if(buf.isRegionActive()) buf.remove();
            buf.insert(ed.copyData());
        },
        DEF_HELP() {
            return "Copies the last copied (or cut) region from the clipboard.";
        });

DEF_CMD(CutRegion, "cut-region",
        DEF_OP() {
            auto& buf = ed.getBuff();
            if(!buf.isRegionActive()) {
                CMBAR_MSG(ed, "No selection to cut!\n");
                return;
            }
            auto del = buf.removeAndCopy();
            ed.setCopyData(del);
        },
        DEF_HELP() {
            return "Cuts the currently active region and copies that data into"
                " the clipboard for a future paste command.";
        });

DEF_CMD(DownloadBuffer, "download-to-buffer",
        DEF_OP() {
            auto& buf = ed.getBuff();
            if(buf.isRegionActive()) buf.remove();
            auto url = ed.prompt("URL: ");
            if(url.empty()) return;
            auto output = downloadUrlToString(url);
            if(output.empty()) return;
            buf.insert(output);
        },
        DEF_HELP() {
            return "Prompt the user for a URL and downloads the contents of"
                " that URL into the buffer.";
        });

DEF_CMD(StartRegion, "start-region",
        DEF_OP() {
            auto& buf = ed.getBuff();
            if(!buf.isRegionActive()) {
                buf.startRegion();
            } else {
                buf.stopRegion();
                buf.startRegion();
            }
        },
        DEF_HELP() { return "Start region from the current cursor position"; });

DEF_CMD(Cancel, "cancel",
        DEF_OP() {
            auto& buf = ed.getBuff();
            if(buf.isRegionActive()) buf.stopRegion();
            if(buf.cursorCount() > 1) buf.clearAllCursorsButFirst();
        },
        DEF_HELP() { return "Cancel all multiple-cursors + active regions"; });

DEF_CMD(CopyRegion, "copy-region",
        DEF_OP() {
            auto& buf = ed.getBuff();
            if(!buf.isRegionActive()) {
                CMBAR_MSG(ed, "No selection to copy!\n");
                return;
            }
            auto cp = buf.regionAsStr();
            ed.setCopyData(cp);
        },
        DEF_HELP() { return "Copy current region into internal clipboard"; });

DEF_CMD(TextSearch, "search",
        DEF_OP() {
            auto& buf = ed.getBuff();
            if(buf.cursorCount() > 1) {
                CMBAR_MSG(ed, "search works only with single cursor!\n");
                return;
            }
            auto pos = buf.saveCursors();
            ISearch is(ed.getWindow());
            is.reset();
            auto ret = ed.prompt("Search: ", nullptr, &is);
            buf.gotoLine(!ret.empty()? is.getIdx() : pos[0].y,
                         ed.getWindow().dim());
        },
        DEF_HELP() { return "Search and jump to the matching location."; });

DEF_CMD(Save, "save-buffer",
        DEF_OP() {
            auto& buf = ed.getBuff();
            if(buf.isRO()) {
                CMBAR_MSG(ed, "save-buffer: Read Only Buffer!\n");
                return;
            }
            ed.saveBuffer(buf);
        },
        DEF_HELP() {
            return "Saves contents of the current buffer to the filesystem.";
        });

DEF_CMD(Pwd, "pwd",
        DEF_OP() {
            const auto& buf = ed.getBuff();
            CMBAR_MSG(ed, "Pwd: %s\n", buf.pwd().c_str());
        },
        DEF_HELP() {
            return "Prints the current working directory of the buffer.";
        });

DEF_CMD(Next, "next-buffer",
        DEF_OP() { ed.incrementCurrBuff(); },
        DEF_HELP() {
            return "Switch to the next buffer";
        });

DEF_CMD(Previous, "prev-buffer",
        DEF_OP() { ed.decrementCurrBuff(); },
        DEF_HELP() {
            return "Switch to the previous buffer";
        });

DEF_CMD(KillThis, "kill-this-buffer",
        DEF_OP() { ed.killCurrBuff(); },
        DEF_HELP() {
            return "Kill the current buffer. It'll check for this buffer to"
                " be modified but unsaved and if so, will prompt for saving.";
        });

DEF_CMD(KillOthers, "kill-other-buffers",
        DEF_OP() { ed.killOtherBuffs(); },
        DEF_HELP() {
            return "Kill other buffers. It'll check for any modified but"
                " unsaved buffers and will prompt them to be saved.";
        });

DEF_CMD(Reload, "reload-buffer",
        DEF_OP() {
            auto& buf = ed.getBuff();
            if(!buf.isModified() ||
               ed.promptYesNo("Buffer modified, still reload? "))
                buf.reload();
        },
        DEF_HELP() {
            return "Reloads the buffer contents from the filesystem. Note that"
                " this will overwrite any modified-but-unsaved changes in the"
                " current buffer!";
        });

} // end namespace BufferOps
} // end namespace teditor
