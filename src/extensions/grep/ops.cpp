#include "core/editor.h"
#include "core/command.h"
#include "core/isearch.h"

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
    //auto& buf = getGrepBuff(ed);
    ///@todo: implement
    //ed.switchToBuff("*grep");
  },
  DEF_HELP() { return "Starts grep-mode buffer, if not already done."; });

} // end namespace ops
} // end namespace grep
} // end namespace teditor
