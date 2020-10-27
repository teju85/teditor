#include "core/editor.h"
#include "core/command.h"
#include <stdio.h>
#include "core/option.h"
#include "core/logger.h"

namespace teditor {
namespace dir {
namespace ops {

/**
 * @page dir_ops Operations supported under dir-mode
 * All operations supported under `dir-mode`.
 *
 * @tableofcontents
 */

/**
 * @page dir_ops
 * @section dirmode-open-file
 * Open the file under the cursor in a new Buffer.
 *
 * @note Available since v0.1.0
 */
DEF_CMD(OpenFile, "dirmode-open-file", "dir_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    const auto& cu = buf.getPoint();
    auto file = buf.dirModeGetFileAtLine(cu.y);
    auto& dir = buf.getFileName();
    if(file == "." || file.empty()) return;
    if(file == "..") {
      ed.load(dirname(dir), 0);
      return;
    }
    ed.load(dir+'/'+file, 0);
  });

/**
 * @page dir_ops
 * @section dirmode-open-special-file
 * Open the file using the program specified in the Option `startProg`.
 *
 * @note Available since v0.1.0
 */
DEF_CMD(OpenSpecialFile, "dirmode-open-special-file", "dir_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    const auto& cu = buf.getPoint();
    auto file = buf.dirModeGetFileAtLine(cu.y);
    auto& dir = buf.getFileName();
    if (file.empty()) return;
    if (file == ".") file = dir;
    else if (file == "..") file = dirname(dir);
    else file = dir + '/' + file;
    auto cmd = Option::get("startProg").getStr() + " " + file;
    CMBAR_MSG(ed, "Running: %s", cmd.c_str());
    check_output(cmd);
  });

/**
 * @page dir_ops
 * @section dirmode-copy-file
 * Copy the file under the cursor by prompting for the name of the new file.
 * Currently, copying of directories is NOT supported.
 *
 * @note Available since v0.1.0
 */
DEF_CMD(CopyFile, "dirmode-copy-file", "dir_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    const auto& cu = buf.getPoint();
    auto file = buf.dirModeGetFileAtLine(cu.y);
    auto& dir = buf.getFileName();
    if(isCurrentOrParentDir(file) || file.empty()) return;
    file = rel2abs(dir, file);
    ///@todo: support copying directories
    if(!isFile(file)) return;
    auto dst = ed.prompt("Dst file: ");
    dst = rel2abs(dir, dst);
    if(!dst.empty()) {
      MESSAGE(ed, "Copied '%s' to '%s'\n", file.c_str(), dst.c_str());
      copyFile(file, dst);
    }
    ed.runCmd("reload-buffer");
  });

/**
 * @page dir_ops
 * @section dirmode-rename-file
 * Rename the file under the cursor by prompting for its new name. This will
 * reload the buffer at the end to reflect the changes.
 *
 * @note Available since v0.1.0
 */
DEF_CMD(RenameFile, "dirmode-rename-file", "dir_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    const auto& cu = buf.getPoint();
    auto file = buf.dirModeGetFileAtLine(cu.y);
    auto& dir = buf.getFileName();
    if(isCurrentOrParentDir(file) || file.empty()) return;
    file = rel2abs(dir, file);
    auto dst = ed.prompt("Dst file: ");
    dst = rel2abs(dir, dst);
    if(!dst.empty()) {
      MESSAGE(ed, "Renamed '%s'->'%s'\n", file.c_str(), dst.c_str());
      rename(file.c_str(), dst.c_str());
    }
    ed.runCmd("reload-buffer");
  });

/**
 * @page dir_ops
 * @section dirmode-delete-file
 * Delete the file under the cursor. This will reload the buffer at the end to
 * reflect the changes.
 *
 * @note Available since v0.1.0
 */
DEF_CMD(DeleteFile, "dirmode-delete-file", "dir_ops", DEF_OP() {
    auto& buf = ed.getBuff();
    auto& dir = buf.getFileName();
    const auto& cu = buf.getPoint();
    auto file = buf.dirModeGetFileAtLine(cu.y);
    if(isCurrentOrParentDir(file) || file.empty()) return;
    auto delFile = rel2abs(dir, file);
    if(delFile.empty()) return;
    if(ed.promptYesNo("Delete " + delFile + "?")) {
      std::string cmd = "rm -rf " + delFile;
      auto res = check_output(cmd);
      MESSAGE(ed, "Deleted file='%s'\n", delFile.c_str());
    }
    ed.runCmd("reload-buffer");
  });

} // end namespace ops
} // end namespace dir
} // end namespace teditor
