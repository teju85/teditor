#include "core/editor.h"
#include "core/command.h"
#include "core/isearch.h"
#include "core/option.h"

namespace teditor {
namespace buffer {
namespace ops {

/**
 * @page buffer_ops Operations on a buffer
 * All operations that can be applied on a Buffer object.
 *
 * @tableofcontents
 */

/**
 * @page buffer_ops
 * @section command-undo command-undo
 * Undo the effect of previously executed command on the current buffer
 *
 * Only the effect of those commands which would have edited the buffer contents
 * will be undo'd. For eg: the command `pwd` has no effect whatsoever on the
 * buffer contents. So, there's no undo for such commands.
 *
 * Available for all modes which enable editing.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  CommandUndo, "command-undo", DEF_OP() {
    if(!ed.getBuff().undo()) CMBAR_MSG(ed, "No further undo information\n");
  },
  DEF_HELP() { return "Undo the previous edition in this buffer."; });

/**
 * @page buffer_ops
 * @section command-redo command-redo
 * Redo the effect of previously undo'd command on the current buffer.
 *
 * Available for all modes which enable editing.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  CommandRedo, "command-redo", DEF_OP() {
    if(!ed.getBuff().redo()) CMBAR_MSG(ed, "No further redo information\n");
  },
  DEF_HELP() { return "Redo the previous undo in this buffer."; });

/**
 * @page buffer_ops
 * @section insert-char .insert-char
 * Inserts a character at the current position of the cursor and moves the
 * cursor one place to the right, or if a region is active, it will remove it
 * first and then inserts this character.
 *
 * @note Preceeding `.` implies that this command is not searchable from the
 *        editor command prompt.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  InsertChar, ".insert-char", DEF_OP() {
    auto& buf = ed.getBuff();
    if(buf.isRegionActive()) ed.runCmd(".backspace-char");
    auto c = (char)ed.getKey();
    buf.insert(c);
  },
  DEF_HELP() { return "Inserts currently pressed char into buffer."; });

/**
 * @page buffer_ops
 * @section backspace-char .backspace-char
 * Deletes the character to the left of the cursor, or if a region is active,
 * removes it instead. As a side-effect, the cursor is moved one place to the
 * left.
 *
 * @note Preceeding `.` implies that this command is not searchable from the
 *        editor command prompt.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(BackspaceChar, ".backspace-char",
        DEF_OP() { ed.getBuff().remove(); },
        DEF_HELP() {
          return "Removes the char to the left of cursor, or if a region is"
            " active, will remove it instead.";
        });

/**
 * @page buffer_ops
 * @section delete-char .delete-char
 * Deletes the character on the cursor, or if a region is active, removes it
 * instead.
 *
 * @note Preceeding `.` implies that this command is not searchable from the
 *        editor command prompt.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(DeleteChar, ".delete-char",
        DEF_OP() { ed.getBuff().remove(true); },
        DEF_HELP() {
          return "Removes the char on the cursor, or if a region is active,"
            " will remove it instead";
        });

/**
 * @page buffer_ops
 * @section kill-line kill-line
 * Removes the rest of the line and copies it to the clipboard.
 *
 * @note Available since v1.0.0
 */
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

/**
 * @page buffer_ops
 * @section keep-lines keep-lines
 * Prompts for a regex and only keeps those lines that match this regex in the
 * whole Buffer.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  KeepLines, "keep-lines", DEF_OP() { keepRemoveLines(ed, true); },
  DEF_HELP() {
    return "Prompts for a PCRE regex and only keeps those lines that match"
      " this regex";
  });

/**
 * @page buffer_ops
 * @section remove-lines remove-lines
 * Prompts for a regex and only removes those lines that match this regex in the
 * whole Buffer.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  RemoveLines, "remove-lines", DEF_OP() { keepRemoveLines(ed, false); },
  DEF_HELP() {
    return "Prompts for a PCRE regex and only removes those lines that match"
      " this regex";
  });

/**
 * @page buffer_ops
 * @section shell-to-buffer shell-to-buffer
 * Prompts the user for a command, executes it inside a shel and inserts the
 * contents from its `stdout` at the current cursor location.
 *
 * Entirety of `stdout` and `stderr` can be seen in the `*messages` Buffer.
 *
 * @note Available since v1.0.0
 */
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

/**
 * @page buffer_ops
 * @section start-region start-region
 * Start a region from the current cursor position. The background color of the
 * active region changes from the default in order to notify the users of the
 * active region in the current Buffer.
 *
 * @note Available since v1.0.0
 */
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

/**
 * @page buffer_ops
 * @section cancel cancel
 * Cancels the following things:
 * 1. active region in the current Buffer, if there's one.
 * 2. ongoing @ref search operation.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  Cancel, "cancel", DEF_OP() {
    auto& buf = ed.getBuff();
    if(buf.isRegionActive()) buf.stopRegion();
  },
  DEF_HELP() { return "Cancel all active regions"; });

/**
 * @page buffer_ops
 * @section paste-region paste-region
 * Copies the last copied (or cut) region from the *internal* clipboard.
 *
 * @note Currently editor has a notion of clipboard that is separate from the
 *        OS provided system clipboard!
 *
 * @note Available since v1.0.0
 */
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

/**
 * @page buffer_ops
 * @section copy-region copy-region
 * Copies the currently active region's contents into the *internal* clipboard
 * for a future @ref paste-region command.
 *
 * @note Currently editor has a notion of clipboard that is separate from the
 *        OS provided system clipboard!
 *
 * @note Available since v1.0.0
 */
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

/**
 * @page buffer_ops
 * @section cut-region cut-region
 * Cuts the currently active region and copies its contents into the *internal*
 * clipboard for a future @ref paste-region command.
 *
 * @note Currently editor has a notion of clipboard that is separate from the
 *        OS provided system clipboard!
 *
 * @note Available since v1.0.0
 */
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

/**
 * @page buffer_ops
 * @section download-to-buffer download-to-buffer
 * Prompt the user for a URL and downloads the contents of that URL into the
 * current Buffer from the current cursor location onwards.
 *
 * @note Available since v1.0.0
 */
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

/**
 * @page buffer_ops
 * @section search search
 * Search for the user supplied string of chars in the current Buffer and jump
 * to a matching location.
 *
 * It starts a multi-line command prompt at the bottom of the current Buffer
 * where the users can input the strings that they want to search for. This
 * multi-line search bar then shows all the matching locations which can be
 * browsed using up/down arrow keys. If the user then presses the Enter key,
 * then the cursor jumps to that line. Press Esc key to cancel this operation.
 *
 * Use the option `iCaseSearch` to enable case insensitive search or not, in
 * your rc file.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  TextSearch, "search", DEF_OP() {
    auto& buf = ed.getBuff();
    auto pos = buf.getPoint();
    ISearch is(ed.getWindow(), Option::get("iCaseSearch").getBool());
    is.reset();
    auto ret = ed.prompt("Search: ", nullptr, &is);
    buf.gotoLine(!ret.empty()? is.getChoiceIdx() : pos.y, ed.getWindow().dim());
  },
  DEF_HELP() { return "Search and jump to the matching location."; });

/**
 * @page buffer_ops
 * @section save-buffer save-buffer
 * Saves the currents of the current Buffer to the filesystem. If the file is at
 * a remote location, then it'll be stored on the filesystem of that remote
 * machine, instead.
 *
 * If there are any unsaved buffers in the current editor session, then while
 * exiting the session, users will be prompted whether to save those buffers or
 * note before the exit.
 *
 * @note Available since v1.0.0
 */
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

/**
 * @page buffer_ops
 * @section pwd pwd
 * Prints the current working directory of the current Buffer, in the status bar
 * below. This command does NOT modify the Buffer contents.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(
  Pwd, "pwd", DEF_OP() {
    const auto& buf = ed.getBuff();
    CMBAR_MSG(ed, "Pwd: %s\n", buf.pwd().c_str());
  },
  DEF_HELP() { return "Prints the current working directory of the buffer."; });

/**
 * @page buffer_ops
 * @section next-buffer next-buffer
 * Switch to the next Buffer in the current sesion.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(Next, "next-buffer",
        DEF_OP() { ed.incrementCurrBuff(); },
        DEF_HELP() { return "Switch to the next buffer"; });

/**
 * @page buffer_ops
 * @section prev-buffer prev-buffer
 * Switch to the previous Buffer in the current sesion.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(Previous, "prev-buffer",
        DEF_OP() { ed.decrementCurrBuff(); },
        DEF_HELP() { return "Switch to the previous buffer"; });

/**
 * @page buffer_ops
 * @section kill-this-buffer kill-this-buffer
 * Kill the current Buffer. It'll check for if it is modified and then prompts
 * the users whether to save it to the filesystem, before killing it.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(KillThis, "kill-this-buffer",
        DEF_OP() { ed.killCurrBuff(); },
        DEF_HELP() {
          return "Kill the current buffer. It'll check for this buffer to"
            " be modified but unsaved and if so, will prompt for saving.";
        });

/**
 * @page buffer_ops
 * @section kill-other-buffers kill-other-buffers
 * Kill all the buffers other than the current one. It'll check for if those
 * buffers are modified and then prompts the users whether to save each of them
 * to the filesystem, before killing it.
 *
 * @note Available since v1.0.0
 */
DEF_CMD(KillOthers, "kill-other-buffers",
        DEF_OP() { ed.killOtherBuffs(); },
        DEF_HELP() {
          return "Kill other buffers. It'll check for any modified but"
            " unsaved buffers and will prompt them to be saved.";
        });

/**
 * @page buffer_ops
 * @section reload-buffer reload-buffer
 * Reloads the Buffer contents from the filesystem. It'll check for if this
 * buffer is modified and then prompts the users to save it to the filesystem,
 * before reloading it.
 *
 * @note This will overwrite any modified-but-unsaved changes in the current
 *        Buffer! It will also erase the undo-redo stack too.
 *
 * @note Available since v1.0.0
 */
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
