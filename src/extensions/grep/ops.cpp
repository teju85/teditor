#include "core/editor.h"
#include "core/command.h"
#include "core/isearch.h"
#include "core/option.h"
#include "core/utils.h"

namespace teditor {
namespace grep {
namespace ops {

Buffer& getGrepBuff(Editor& ed) {
  bool newOne;
  auto& buf = ed.getBuff("*grep", true, newOne);
  if (newOne) {
    buf.setMode(Mode::createMode(Mode::inferMode(buf.bufferName(), false)));
  }
  return buf;
}

DEF_CMD(
  Grep, "grep", DEF_OP() {
    auto cmd = ed.prompt("Run grep (like this): ", nullptr, nullptr,
                         Option::get("grepCmd").getStr());
    if (cmd.empty()) {
      CMBAR_MSG(ed, "grep: nothing to run!\n");
      return;
    }
    CMBAR_MSG(ed, "grep cmd = %s\n", cmd.c_str());
    auto res = check_output(cmd);
    if (res.status != 0) {
      CMBAR_MSG(ed, "grep failed. Exit status = %d\n", res.status);
      MESSAGE(ed, "cmd = %s\nerr = %s\n", cmd.c_str(), res.error.c_str());
      return;
    }
    auto& buf = getGrepBuff(ed);
    buf.insert("Grep\nCommand: " + cmd + "\npwd: " + buf.pwd() + "\n\n");
    buf.insert(res.output);
    buf.begin();
    ed.switchToBuff("*grep");
  },
  DEF_HELP() { return "Starts grep-mode buffer, if not already done."; });

DEF_CMD(
  GrepFindFile, "grep-find-file", DEF_OP() {
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
    auto pwd = buf.at(2).get().substr(5);
    if (pwd == ".") pwd = buf.pwd();
    file = pwd + '/' + file;
    ed.load(file, lineNum);
  },
  DEF_HELP() { return "Starts grep-mode buffer, if not already done."; });

} // end namespace ops
} // end namespace grep
} // end namespace teditor
