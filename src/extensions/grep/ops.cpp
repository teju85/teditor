#include "core/editor.h"
#include "core/command.h"
#include "core/isearch.h"
#include "core/option.h"
#include "core/utils.h"

namespace teditor {
namespace grep {
namespace ops {

/**
 * @page grep_ops grep-mode
 * All operations supported under `grep-mode`.
 *
 * @tableofcontents
 *
 *
 * @section grep
 * Prompts for the `grep` command to be run, executes the command on the given
 * file/folder. Then starts `grep-mode` buffer, if not already done and puts the
 * output of this command in this buffer for your perusal.
 *
 * @note Available since v1.6.0
 *
 *
 * @section grep-find-file
 * During the `grep-mode`, this opens up the file that the cursor is currently
 * on and jumps to the line number as seen in the output of grep.
 *
 * @note Available since v1.6.0
 */

Buffer& getGrepBuff(Editor& ed) {
  bool newOne;
  auto& buf = ed.getBuff("*grep", true, newOne);
  if (newOne)
    buf.setMode(Mode::createMode(Mode::inferMode(buf.bufferName(), false)));
  return buf;
}

DEF_CMD(Grep, "grep", "grep_ops", DEF_OP() {
    auto cmd = ed.prompt("Run grep (like this): ", nullptr, nullptr,
                         Option::get("grepCmd").getStr());
    if (cmd.empty()) {
      CMBAR_MSG(ed, "grep: nothing to run!\n");
      return;
    }
    CMBAR_MSG(ed, "grep cmd = %s\n", cmd.c_str());
    auto pwd = ed.getBuff().pwd();
    if (cmd.back() == '.') {
      cmd.pop_back();
      cmd += pwd;
    }
    auto res = check_output(cmd);
    if (res.status != 0) {
      CMBAR_MSG(ed, "grep failed. Exit status = %d\n", res.status);
      MESSAGE(ed, "cmd = %s\nerr = %s\n", cmd.c_str(), res.error.c_str());
      return;
    }
    auto& buf = getGrepBuff(ed);
    buf.clear();
    buf.insert("Grep\nCommand: " + cmd + "\npwd: " + pwd + "\n\n");
    buf.insert(res.output);
    buf.begin();
    ed.switchToBuff("*grep");
  });

DEF_CMD(GrepFindFile, "grep-find-file", "grep_ops", DEF_OP() {
    auto& buf = getGrepBuff(ed);
    const auto& cu = buf.getPoint();
    if (cu.y < 4) {
      CMBAR_MSG(ed, "grep: Point not on files!\n");
      return;
    }
    const auto& line = buf.at(cu.y).get();
    auto fileEnd = line.find_first_of(':');
    if (fileEnd == std::string::npos) {
      CMBAR_MSG(ed, "grep: bad line!\n");
      return;
    }
    auto lineEnd = line.find_first_of(':', fileEnd + 1);
    if (lineEnd == std::string::npos) {
      CMBAR_MSG(ed, "grep: bad line!\n");
      return;
    }
    auto file = line.substr(0, fileEnd);
    // grep outputs 1-based line numbering
    auto lineNum = str2num(line.substr(fileEnd + 1, lineEnd = fileEnd)) - 1;
    if (file[0] != '/') {
      auto pwd = buf.at(2).get().substr(5);
      if (pwd == ".") pwd = buf.pwd();
      file = pwd + '/' + file;
    }
    ed.load(file, lineNum);
  });

} // end namespace ops
} // end namespace grep
} // end namespace teditor
