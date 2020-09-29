#include "core/editor.h"
#include "core/command.h"
#include "mode.h"

namespace teditor {
namespace todo {
namespace ops {

Buffer& getTodoShowBuff(Editor& ed) {
  bool newOne;
  auto& buf = ed.getBuff("*todo", true, newOne);
  if (newOne)
    buf.setMode(Mode::createMode(Mode::inferMode(buf.bufferName(), false)));
  return buf;
}

DEF_CMD(
  TodoShowCmd, "todo::show", DEF_OP() {
    auto& currBuf = ed.getBuff();
    if (currBuf.modeName() != "todo") {
      CMBAR_MSG(ed, "Current buffer doesn't have a todo file in it!\n");
      return;
    }
    //auto& buf = getTodoShowBuff(ed);
    //auto* mode = buf.getMode<todo::TodoShowMode>("todo-show");
    ed.switchToBuff("*todo");
  },
  DEF_HELP() { return "Starts todo-mode buffer, if not already done."; });

} // end namespace ops
} // end namespace todo
} // end namespace teditor
