#include "core/editor.h"
#include "core/command.h"
#include "mode.h"

namespace teditor {
namespace watch {
namespace ops {

Buffer& getWatchBuff(Editor& ed) {
  bool newOne;
  auto& buf = ed.getBuff("*watch", true, newOne);
  if (newOne) {
    buf.setMode(Mode::createMode(Mode::inferMode(buf.bufferName(), false)));
  }
  return buf;
}

DEF_CMD(
  Watch, "watch", DEF_OP() {
    auto& buf = getWatchBuff(ed);
    ed.switchToBuff("*watch");
    auto* mode = buf.getMode<watch::WatchMode>("watch");
    auto cmd = ed.prompt("Command to watch: ");
    if (cmd.empty()) return;
    mode->start(&buf, cmd);
  },
  DEF_HELP() { return "Start a watch command."; });

DEF_CMD(
  WatchSleep, "watch-sleep", DEF_OP() {
    auto& buf = getWatchBuff(ed);
    ed.switchToBuff("*watch");
    auto* mode = buf.getMode<watch::WatchMode>("watch");
    auto cmd = ed.prompt("Command to watch: ");
    if (cmd.empty()) return;
    auto sleep = ed.prompt("Sleep time (in ms) between retries: ");
    if (sleep.empty()) return;
    mode->start(&buf, cmd, str2num(sleep));
  },
  DEF_HELP() { return "Start a watch command."; });

DEF_CMD(
  WatchStop, "watch::stop", DEF_OP() {
    auto& buf = getWatchBuff(ed);
    ed.switchToBuff("*watch");
    auto* mode = buf.getMode<watch::WatchMode>("watch");
    mode->stop();
  },
  DEF_HELP() { return "Stops the underlying watch command."; });

DEF_CMD(
  WatchRestart, "watch::restart", DEF_OP() {
    auto& buf = getWatchBuff(ed);
    ed.switchToBuff("*watch");
    auto* mode = buf.getMode<watch::WatchMode>("watch");
    mode->restart();
  },
  DEF_HELP() { return "Restarts the underlying watch command."; });

} // end namespace ops
} // end namespace watch
} // end namespace teditor
