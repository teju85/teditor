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

/**
 * @page todo_ops Operations supported under todo-mode
 * All operations supported under `todo-mode`.
 *
 * @tableofcontents
 */

Buffer& getTodoShowBuff(Editor& ed) {
  bool newOne;
  auto& buf = ed.getBuff("*todo", true, newOne);
  if (newOne)
    buf.setMode(Mode::createMode(Mode::inferMode(buf.bufferName(), false)));
  return buf;
}

/**
 * @page todo_ops
 * @section todo-open
 * Open the todo file in a new Buffer, if not already done, and switch to it.
 * The location of this file can be configured with the Option `todo:file`.
 *
 * @note Available since v1.8.0.
 */
DEF_CMD(TodoOpen, "todo-open", "ledger_ops", DEF_OP() {
    auto todoFile = Option::get("todo:file").getStr();
    ed.load(todoFile, 0);
  });

void showTodosFor(Buffer& buf, const TimePoint& start, const TimePoint& end) {
  auto todoFile = Option::get("todo:file").getStr();
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
      ss << timeToDateStr(m.pt) << " ---> " << items[m.idx].description
         << "\n";
    }
  }
  buf.insert(ss.str());
}

/**
 * @page todo_ops
 * @section todo-show-this-week
 * Show all todo's to be done in the current week. Current week starts from
 * Sunday.
 *
 * @note Available since v1.8.0.
 */
DEF_CMD(TodoShowThisWeek, "todo-show-this-week", "ledger_ops", DEF_OP() {
    auto& buf = getTodoShowBuff(ed);
    TimePoint s;
    TimePoint e;
    weekFor(s, e, std::chrono::system_clock::now());
    showTodosFor(buf, s, e);
    ed.switchToBuff("*todo");
  });

} // end namespace ops
} // end namespace todo
} // end namespace teditor
