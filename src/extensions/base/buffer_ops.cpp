#include "core/editor.h"
#include "core/command.h"
#include "core/isearch.h"
#include "core/option.h"

namespace teditor {
namespace buffer {
namespace ops {

/**
 * @page buffer_ops Buffer
 * All operations that can be applied on a Buffer object.
 *
 * @tableofcontents
 *
 *
 * @section command-undo
 * Undo the effect of previously executed command on the current buffer
 *
 * Only the effect of those commands which would have edited the buffer contents
 * will be undo'd. For eg: the command `pwd` has no effect whatsoever on the
 * buffer contents. So, there's no undo for such commands.
 *
 * Available for all modes which enable editing.
 *
 * @note Available since v1.0.0
 *
 *
 * @section command-redo
 * Redo the effect of previously undo'd command on the current buffer.
 *
 * Available for all modes which enable editing.
 *
 * @note Available since v1.0.0
 *
 *
 * @section insert-char .insert-char
 * Inserts a character at the current position of the cursor and moves the
 * cursor one place to the right, or if a region is active, it will remove it
 * first and then inserts this character.
 *
 * @note Preceeding `.` implies that this command is not searchable from the
 *        editor command prompt.
 *
 * @note Available since v1.0.0
 *
 *
 * @section backspace-char .backspace-char
 * Deletes the character to the left of the cursor, or if a region is active,
 * removes it instead. As a side-effect, the cursor is moved one place to the
 * left.
 *
 * @note Preceeding `.` implies that this command is not searchable from the
 *        editor command prompt.
 *
 * @note Available since v1.0.0
 *
 *
 * @section delete-char .delete-char
 * Deletes the character on the cursor, or if a region is active, removes it
 * instead.
 *
 * @note Preceeding `.` implies that this command is not searchable from the
 *        editor command prompt.
 *
 * @note Available since v1.0.0
 *
 *
 * @section kill-line
 * Removes the rest of the line and copies it to the clipboard.
 *
 * @note Available since v1.0.0
 *
 *
 * @section keep-lines
 * Prompts for a regex and only keeps those lines that match this regex in the
 * whole Buffer.
 *
 * @note Available since v1.0.0
 *
 *
 * @section remove-lines
 * Prompts for a regex and only removes those lines that match this regex in the
 * whole Buffer.
 *
 * @note Available since v1.0.0
 *
 *
 * @section shell-to-buffer shell-to-buffer
 * Prompts the user for a command, executes it inside a shel and inserts the
 * contents from its `stdout` at the current cursor location.
 *
 * Entirety of `stdout` and `stderr` can be seen in the `*messages` Buffer.
 *
 * @note Available since v1.0.0
 *
 *
 * @section start-region
 * Start a region from the current cursor position. The background color of the
 * active region changes from the default in order to notify the users of the
 * active region in the current Buffer.
 *
 * @note Available since v1.0.0
 *
 *
 * @section cancel
 * Cancels the following things:
 * 1. active region in the current Buffer, if there's one.
 * 2. ongoing @ref search operation.
 *
 * @note Available since v1.0.0
 *
 *
 * @section paste-region
 * Copies the last copied (or cut) region from the *internal* clipboard.
 *
 * @note Currently editor has a notion of clipboard that is separate from the
 *        OS provided system clipboard!
 *
 * @note Available since v1.0.0
 *
 *
 * @section copy-region
 * Copies the currently active region's contents into the *internal* clipboard
 * for a future @ref paste-region command.
 *
 * @note Currently editor has a notion of clipboard that is separate from the
 *        OS provided system clipboard!
 *
 * @note Available since v1.0.0
 *
 *
 * @section cut-region
 * Cuts the currently active region and copies its contents into the *internal*
 * clipboard for a future @ref paste-region command.
 *
 * @note Currently editor has a notion of clipboard that is separate from the
 *        OS provided system clipboard!
 *
 * @note Available since v1.0.0
 *
 *
 * @section download-to-buffer
 * Prompt the user for a URL and downloads the contents of that URL into the
 * current Buffer from the current cursor location onwards.
 *
 * @note Available since v1.0.0
 *
 *
 * @section search
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
 *
 *
 * @section save-buffer
 * Saves the currents of the current Buffer to the filesystem. If the file is at
 * a remote location, then it'll be stored on the filesystem of that remote
 * machine, instead.
 *
 * If there are any unsaved buffers in the current editor session, then while
 * exiting the session, users will be prompted whether to save those buffers or
 * note before the exit.
 *
 * @note Available since v1.0.0
 *
 *
 * @section pwd
 * Prints the current working directory of the current Buffer, in the status bar
 * below. This command does NOT modify the Buffer contents.
 *
 * @note Available since v1.0.0
 *
 *
 * @section next-buffer
 * Switch to the next Buffer in the current sesion.
 *
 * @note Available since v1.0.0
 *
 *
 * @section prev-buffer
 * Switch to the previous Buffer in the current sesion.
 *
 * @note Available since v1.0.0
 *
 *
 * @section kill-this-buffer
 * Kill the current Buffer. It'll check for if it is modified and then prompts
 * the users whether to save it to the filesystem, before killing it.
 *
 * @note Available since v1.0.0
 *
 *
 * @section kill-other-buffers
 * Kill all the buffers other than the current one. It'll check for if those
 * buffers are modified and then prompts the users whether to save each of them
 * to the filesystem, before killing it.
 *
 * @note Available since v1.0.0
 *
 *
 * @section reload-buffer
 * Reloads the Buffer contents from the filesystem. It'll check for if this
 * buffer is modified and then prompts the users to save it to the filesystem,
 * before reloading it.
 *
 * @note This will overwrite any modified-but-unsaved changes in the current
 *        Buffer! It will also erase the undo-redo stack too.
 *
 * @note Available since v1.0.0
 */

DEF_CMD(CommandUndo, "command-undo", "buffer_ops", DEF_OP() {
    if(!ed.getBuff().undo()) CMBAR_MSG(ed, "No further undo information\n");
  });

DEF_CMD(CommandRedo, "command-redo", "buffer_ops", DEF_OP() {
    if(!ed.getBuff().redo()) CMBAR_MSG(ed, "No further redo information\n");
  });

DEF_CMD(InsertChar, ".insert-char", "buffer_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    if(buf.isRegionActive()) ed.runCmd(".backspace-char");
    auto c = (char)ed.getKey();
    buf.insert(c);
  });

DEF_CMD(BackspaceChar, ".backspace-char", "buffer_ops",
        DEF_OP() { ed.getBuff().remove(); });

DEF_CMD(DeleteChar, ".delete-char", "buffer_ops",
        DEF_OP() { ed.getBuff().remove(true); });

DEF_CMD(KillLine, "kill-line", "buffer_ops", DEF_OP() {
    auto del = ed.getBuff().killLine();
    ed.setClipboard(del);
    CMBAR_MSG(ed, "Line killed\n");
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

DEF_CMD(KeepLines, "keep-lines", "buffer_ops",
        DEF_OP() { keepRemoveLines(ed, true); });

DEF_CMD(RemoveLines, "remove-lines", "buffer_ops",
        DEF_OP() { keepRemoveLines(ed, false); });

DEF_CMD(ShellToBuffer, "shell-to-buffer", "buffer_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    if(buf.isRegionActive()) buf.remove();
    auto cmd = ed.prompt("Shell Command: ");
    if(cmd.empty()) return;
    MESSAGE(ed, "Shell Command: %s\n", cmd.c_str());
    auto res = check_output(cmd);
    if(res.output.empty()) return;
    buf.insert(res.output);
    MESSAGE(ed, "Exit Code: %d\nError: %s\n", res.status, res.error.c_str());
  });

DEF_CMD(StartRegion, "start-region", "buffer_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    if(!buf.isRegionActive()) {
      buf.startRegion();
    } else {
      buf.stopRegion();
      buf.startRegion();
    }
  });

DEF_CMD(Cancel, "cancel", "buffer_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    if(buf.isRegionActive()) buf.stopRegion();
  });

DEF_CMD(PasteRegion, "paste-region", "buffer_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    if(buf.isRegionActive()) buf.remove();
    auto copy = ed.clipboard();
    if (!copy.empty()) buf.insert(copy);
  });

DEF_CMD(CopyRegion, "copy-region", "buffer_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    if(!buf.isRegionActive()) {
      CMBAR_MSG(ed, "No selection to copy!\n");
      return;
    }
    auto cp = buf.regionAsStr();
    ed.setClipboard(cp);
    buf.stopRegion();
    CMBAR_MSG(ed, "Copy done\n");
  });

DEF_CMD(CutRegion, "cut-region", "buffer_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    if(!buf.isRegionActive()) {
      CMBAR_MSG(ed, "No selection to cut!\n");
      return;
    }
    auto del = buf.removeAndCopy();
    ed.setClipboard(del);
    CMBAR_MSG(ed, "Cut done\n");
  });

DEF_CMD(DownloadBuffer, "download-to-buffer", "buffer_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    if(buf.isRegionActive()) buf.remove();
    auto url = ed.prompt("URL: ");
    if(url.empty()) return;
    auto output = downloadUrlToString(url, Option::get("dnldProg").getStr(),
                                      Option::get("dnldProgOpts").getStr());
    if(output.empty()) return;
    buf.insert(output);
  });

DEF_CMD(TextSearch, "search", "buffer_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    auto pos = buf.getPoint();
    ISearch is(ed.getWindow(), Option::get("iCaseSearch").getBool());
    is.reset();
    auto ret = ed.prompt("Search: ", nullptr, &is);
    buf.gotoLine(!ret.empty()? is.getChoiceIdx() : pos.y, ed.getWindow().dim());
  });

DEF_CMD(Save, "save-buffer", "buffer_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    if(buf.isRO()) {
      CMBAR_MSG(ed, "save-buffer: Read Only Buffer!\n");
      return;
    }
    ed.saveBuffer(buf);
  });

DEF_CMD(Pwd, "pwd", "buffer_ops", DEF_OP() {
    const auto& buf = ed.getBuff();
    CMBAR_MSG(ed, "Pwd: %s\n", buf.pwd().c_str());
  });

DEF_CMD(Next, "next-buffer", "buffer_ops",
        DEF_OP() { ed.incrementCurrBuff(); });

DEF_CMD(Previous, "prev-buffer", "buffer_ops",
        DEF_OP() { ed.decrementCurrBuff(); });

DEF_CMD(KillThis, "kill-this-buffer", "buffer_ops",
        DEF_OP() { ed.killCurrBuff(); });

DEF_CMD(KillOthers, "kill-other-buffers", "buffer_ops",
        DEF_OP() { ed.killOtherBuffs(); });

DEF_CMD(Reload, "reload-buffer", "buffer_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    if(!buf.isModified() ||
       ed.promptYesNo("Buffer modified, still reload? "))
      buf.reload();
  });

} // end namespace ops
} // end namespace buffer
} // end namespace teditor
