#include "core/editor.h"
#include "core/command.h"
#include "core/isearch.h"
#include "core/option.h"

namespace teditor {
namespace buffer {
namespace ops {

DEF_CMD(
  CommandUndo, "command-undo", DEF_OP() {
    if(!ed.getBuff().undo()) CMBAR_MSG(ed, "No further undo information\n");
  },
  DEF_HELP() { return "Undo the previous edition in this buffer."; });

DEF_CMD(
  CommandRedo, "command-redo", DEF_OP() {
    if(!ed.getBuff().redo()) CMBAR_MSG(ed, "No further redo information\n");
  },
  DEF_HELP() { return "Redo the previous undo in this buffer."; });

DEF_CMD(
  InsertChar, ".insert-char", DEF_OP() {
    auto& buf = ed.getBuff();
    if(buf.isRegionActive()) ed.runCmd(".backspace-char");
    auto c = (char)ed.getKey();
    buf.insert(c);
  },
  DEF_HELP() { return "Inserts currently pressed char into buffer."; });

DEF_CMD(BackspaceChar, ".backspace-char",
        DEF_OP() { ed.getBuff().remove(); },
        DEF_HELP() {
          return "Removes the char to the left of cursor, or if a region is"
            " active, will remove it instead.";
        });

DEF_CMD(DeleteChar, ".delete-char",
        DEF_OP() { ed.getBuff().remove(true); },
        DEF_HELP() {
          return "Removes the char on the cursor, or if a region is active,"
            " will remove it instead";
        });

DEF_CMD(
  KillLine, "kill-line", DEF_OP() {
    auto del = ed.getBuff().killLine();
    ed.setClipboard(del);
    CMBAR_MSG(ed, "Line killed\n");
  },
  DEF_HELP() {
    return "Removes the rest of the line and copies it to the clipboard.";
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

void keepRemoveLines(Editor& ed, bool keep) {
  auto& buf = ed.getBuff();
  auto regex = ed.prompt(keep ? "Keep Regex: " : "Remove Regex: ");
  if (regex.empty()) return;
  parser::NFA nfa(regex);
  auto before = buf.length();
  buf.keepRemoveLines(nfa, keep);
  auto after = buf.length();
  CMBAR_MSG(ed, "Removed %d lines\n", before - after);
}

DEF_CMD(
  KeepLines, "keep-lines", DEF_OP() { keepRemoveLines(ed, true); },
  DEF_HELP() {
    return "Prompts for a PCRE regex and only keeps those lines that match"
      " this regex";
  });

DEF_CMD(
  RemoveLines, "remove-lines", DEF_OP() { keepRemoveLines(ed, false); },
  DEF_HELP() {
    return "Prompts for a PCRE regex and only removes those lines that match"
      " this regex";
  });

DEF_CMD(
  ShellToBuffer, "shell-to-buffer", DEF_OP() {
    auto& buf = ed.getBuff();
    if(buf.isRegionActive()) buf.remove();
    auto cmd = ed.prompt("Shell Command: ");
    if(cmd.empty()) return;
    MESSAGE(ed, "Shell Command: %s\n", cmd.c_str());
    auto res = check_output(cmd);
    if(res.output.empty()) return;
    buf.insert(res.output);
    MESSAGE(ed, "Exit Code: %d\nError: %s\n", res.status, res.error.c_str());
  },
  DEF_HELP() {
    return "Prompts the user for a command, executes it inside a shall"
      " and inserts its output into the current buffer.";
  });

DEF_CMD(
  PasteRegion, "paste-region", DEF_OP() {
    auto& buf = ed.getBuff();
    if(buf.isRegionActive()) buf.remove();
    auto copy = ed.clipboard();
    if (!copy.empty()) buf.insert(copy);
  },
  DEF_HELP() {
    return "Copies the last copied (or cut) region from the clipboard.";
  });

DEF_CMD(
  CutRegion, "cut-region", DEF_OP() {
    auto& buf = ed.getBuff();
    if(!buf.isRegionActive()) {
      CMBAR_MSG(ed, "No selection to cut!\n");
      return;
    }
    auto del = buf.removeAndCopy();
    ed.setClipboard(del);
    CMBAR_MSG(ed, "Cut done\n");
  },
  DEF_HELP() {
    return "Cuts the currently active region and copies that data into"
      " the clipboard for a future paste command.";
  });

DEF_CMD(
  DownloadBuffer, "download-to-buffer", DEF_OP() {
    auto& buf = ed.getBuff();
    if(buf.isRegionActive()) buf.remove();
    auto url = ed.prompt("URL: ");
    if(url.empty()) return;
    auto output = downloadUrlToString(url, Option::get("dnldProg").getStr(),
                                      Option::get("dnldProgOpts").getStr());
    if(output.empty()) return;
    buf.insert(output);
  },
  DEF_HELP() {
    return "Prompt the user for a URL and downloads the contents of"
      " that URL into the buffer.";
  });

DEF_CMD(
  StartRegion, "start-region", DEF_OP() {
    auto& buf = ed.getBuff();
    if(!buf.isRegionActive()) {
      buf.startRegion();
    } else {
      buf.stopRegion();
      buf.startRegion();
    }
  },
  DEF_HELP() { return "Start region from the current cursor position"; });

DEF_CMD(
  Cancel, "cancel", DEF_OP() {
    auto& buf = ed.getBuff();
    if(buf.isRegionActive()) buf.stopRegion();
  },
  DEF_HELP() { return "Cancel all active regions"; });

DEF_CMD(
  CopyRegion, "copy-region", DEF_OP() {
    auto& buf = ed.getBuff();
    if(!buf.isRegionActive()) {
      CMBAR_MSG(ed, "No selection to copy!\n");
      return;
    }
    auto cp = buf.regionAsStr();
    ed.setClipboard(cp);
    buf.stopRegion();
    CMBAR_MSG(ed, "Copy done\n");
  },
  DEF_HELP() { return "Copy current region into internal clipboard"; });

DEF_CMD(
  TextSearch, "search", DEF_OP() {
    auto& buf = ed.getBuff();
    auto pos = buf.getPoint();
    ISearch is(ed.getWindow(), Option::get("iCaseSearch").getBool());
    is.reset();
    auto ret = ed.prompt("Search: ", nullptr, &is);
    buf.gotoLine(!ret.empty()? is.getIdx() : pos.y, ed.getWindow().dim());
  },
  DEF_HELP() { return "Search and jump to the matching location."; });

DEF_CMD(
  Save, "save-buffer", DEF_OP() {
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

DEF_CMD(
  Pwd, "pwd", DEF_OP() {
    const auto& buf = ed.getBuff();
    CMBAR_MSG(ed, "Pwd: %s\n", buf.pwd().c_str());
  },
  DEF_HELP() { return "Prints the current working directory of the buffer."; });

DEF_CMD(Next, "next-buffer",
        DEF_OP() { ed.incrementCurrBuff(); },
        DEF_HELP() { return "Switch to the next buffer"; });

DEF_CMD(Previous, "prev-buffer",
        DEF_OP() { ed.decrementCurrBuff(); },
        DEF_HELP() { return "Switch to the previous buffer"; });

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

DEF_CMD(
  Reload, "reload-buffer", DEF_OP() {
    auto& buf = ed.getBuff();
    if(!buf.isModified() ||
       ed.promptYesNo("Buffer modified, still reload? "))
      buf.reload();
  },
  DEF_HELP() {
    return "Reloads the buffer contents from the filesystem. Note that this"
      " will overwrite any modified-but-unsaved changes in current buffer!";
  });

} // end namespace ops
} // end namespace buffer
} // end namespace teditor
