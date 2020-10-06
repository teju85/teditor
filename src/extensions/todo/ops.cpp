#include "core/editor.h"
#include "core/command.h"
#include "core/option.h"

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
  TodoOpen, "todo", DEF_OP() {
    auto todoFile = Option::get("todo::file").getStr();
    ed.load(todoFile, 0);
  },
  DEF_HELP() { return "Opens the todo file in a new buffer."; });

} // end namespace ops
} // end namespace todo
} // end namespace teditor
