#include "core/editor.h"
#include "core/command.h"
#include "core/isearch.h"


namespace teditor {
namespace BufferOps {

DEF_CMD(CommandUndo, "command-undo",
        DEF_OP() { ed.getBuff().undo(); },
        DEF_HELP() { return "Undo the previous edition in this buffer."; });

DEF_CMD(CommandRedo, "command-redo",
        DEF_OP() { ed.getBuff().redo(); },
        DEF_HELP() { return "Redo the previous undo in this buffer."; });

DEF_CMD(InsertChar, "insert-char",
        DEF_OP() {
            auto& buf = ed.getBuff();
            char c = ed.getInput().mk.getKey();
            buf.insert(c);
            if(buf.isRegionActive()) ed.runCmd("backspace-char");
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
//     auto& ed = Editor::getInstance();
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
//     auto& ed = Editor::getInstance();
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
//     auto& ed = Editor::getInstance();
//     auto& buf = ed.getBuff();
//     if(buf.cursorCount() > 1) {
//         CMBAR_MSG("keep-lines works only with single cursor!\n");
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
//     auto& ed = Editor::getInstance();
//     auto& buf = ed.getBuff();
//     if(buf.cursorCount() > 1) {
//         CMBAR_MSG("remove-lines works only with single cursor!\n");
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

// CMD_UNDO3(ShellToBuffer, "shell-to-buffer", Positions before, Positions after,
//           std::string output) {
//     auto& ed = Editor::getInstance();
//     if(buf.isRegionActive())
//         buf.stopRegion();
//     auto& mlb = ed.getBuff();
//     switch(type) {
//     case CMD_UNDO:
//         mlb.restoreCursors(before);
//         mlb.removeRegion(before, after);
//         break;
//     case CMD_REDO:
//         mlb.restoreCursors(before);
//         mlb.insert(output.c_str());
//         break;
//     default:
//         auto cmd = ed.prompt("Shell Command: ");
//         if(cmd.empty()) {
//             canIundo = false;
//             return;
//         }
//         auto res = check_output(cmd);
//         output = res.output;
//         if(output.empty()) {
//             canIundo = false;
//             return;
//         }
//         before = mlb.saveCursors();
//         mlb.insert(output.c_str());
//         after = mlb.saveCursors();
//     };
// }

// CMD_UNDO3(PasteRegion, "paste-region", Strings del, Positions before,
//           Positions after) {
//     auto& ed = Editor::getInstance();
//     auto& buf = ed.getBuff();
//     if(type == CMD_FRESH) {
//         if(!ed.hasCopy()) {
//             canIundo = false;
//             CMBAR_MSG("No selection to paste!\n");
//             return;
//         }
//         if(buf.isRegionActive())
//             ed.runCmd("remove-region");
//     }
//     switch(type) {
//     case CMD_UNDO:
//         buf.restoreCursors(before);
//         buf.removeRegion(before, after);
//         break;
//     case CMD_REDO:
//         buf.restoreCursors(before);
//         buf.insert(del);
//         break;
//     default:
//         before = buf.saveCursors();
//         del = ed.copyData();
//         buf.insert(del);
//         after = buf.saveCursors();
//     };
// }

// ///@todo: what if before is before regs!?
// CMD_UNDO3(CutRegion, "cut-region", Strings del, Positions before,
//           Positions regs) {
//     auto& ed = Editor::getInstance();
//     auto& buf = ed.getBuff();
//     if(type == CMD_FRESH && !buf.isRegionActive()) {
//         canIundo = false;
//         CMBAR_MSG("No selection to cut!\n");
//         return;
//     }
//     switch(type) {
//     case CMD_UNDO:
//         buf.restoreCursors(regs);
//         buf.insert(del);
//         break;
//     case CMD_REDO:
//         del = buf.removeRegion(regs, before);
//         buf.restoreCursors(regs);
//         break;
//     default:
//         before = buf.saveCursors();
//         regs = buf.getRegionLocs();
//         del = buf.removeRegion(regs, before);
//         ed.setCopyData(del);
//         buf.restoreCursors(regs);
//     };
//     buf.stopRegion();
// }

// CMD_UNDO3(DownloadBuffer, "download-to-buffer", Positions before, Positions after,
//           std::string output) {
//     auto& ed = Editor::getInstance();
//     if(buf.isRegionActive())
//         buf.stopRegion();
//     auto& mlb = ed.getBuff();
//     switch(type) {
//     case CMD_UNDO:
//         mlb.restoreCursors(before);
//         mlb.removeRegion(before, after);
//         break;
//     case CMD_REDO:
//         mlb.restoreCursors(before);
//         mlb.insert(output.c_str());
//         break;
//     default:
//         auto url = ed.prompt("URL: ");
//         if(url.empty()) {
//             canIundo = false;
//             return;
//         }
//         auto output = downloadUrlToString(url);
//         if(output.empty()) {
//             canIundo = false;
//             return;
//         }
//         before = mlb.saveCursors();
//         mlb.insert(output.c_str());
//         after = mlb.saveCursors();
//     };
// }

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
                CMBAR_MSG("No selection to copy!\n");
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
                CMBAR_MSG("search works only with single cursor!\n");
                return;
            }
            auto pos = buf.saveCursors();
            ISearch is(buf);
            is.reset();
            auto ret = ed.prompt("Search: ", nullptr, &is);
            buf.gotoLine(!ret.empty()? is.getIdx() : pos[0].y);
        },
        DEF_HELP() { return "Search and jump to the matching location."; });

DEF_CMD(Save, "save-buffer",
        DEF_OP() {
            auto& buf = ed.getBuff();
            if(buf.isRO()) {
                CMBAR_MSG("save-buffer: Read Only Buffer!\n");
                return;
            }
            buf.save();
        },
        DEF_HELP() {
            return "Saves contents of the current buffer to the filesystem.";
        });

DEF_CMD(Pwd, "pwd",
        DEF_OP() {
            const auto& buf = ed.getBuff();
            CMBAR_MSG("Pwd: %s\n", buf.pwd().c_str());
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
            if(buf.isModified() &&
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
