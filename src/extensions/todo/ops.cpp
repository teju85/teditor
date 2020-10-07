#include "core/editor.h"
#include "core/command.h"
#include "core/option.h"
#include <sstream>
#include "core/time_utils.h"
#include "parser.h"
#include "objects.h"

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

void showTodosFor(Buffer& buf, const TimePoint& start, const TimePoint& end) {
  auto todoFile = Option::get("todo::file").getStr();
  Parser p(todoFile);
  buf.clear();
  std::stringstream ss;
  ss << "# TODO's for " << timeToDateStr(start) << " to " << timeToDateStr(end)
     << "\n###############################################################\n\n";
  const auto& items = p.items();
  auto matches = findMatchesIn(items, start, end);
  if (matches.empty()) {
    ss << "NONE\n";
  } else {
    for (const auto& m : matches) {
      ss << timeToDateStr(m.pt) << " ---> \"" << items[m.idx].description
         << "\n";
    }
  }
  buf.insert(ss.str());
}

DEF_CMD(
  TodoShowThisWeek, "todo::show-this-week", DEF_OP() {
    auto& buf = getTodoShowBuff(ed);
    showTodosFor(buf, timeFromStr("2020-10-01"), timeFromStr("2020-10-08"));
    ed.switchToBuff("*todo");
  },
  DEF_HELP() { return "Opens the todo file in a new buffer."; });

} // end namespace ops
} // end namespace todo
} // end namespace teditor
