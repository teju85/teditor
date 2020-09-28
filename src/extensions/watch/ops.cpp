#include "core/editor.h"
#include "core/command.h"
#include "mode.h"
#include "core/utils.h"

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
    CMBAR_MSG(ed, "Starting watch on '%s'...\n", cmd.c_str());
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
    CMBAR_MSG(ed, "Starting watch on '%s' with sleep-time=%s ms...\n",
              cmd.c_str(), sleep.c_str());
    mode->start(&buf, cmd, str2num(sleep));
  },
  DEF_HELP() { return "Start a watch command."; });

DEF_CMD(
  WatchStop, "watch::stop", DEF_OP() {
    auto& buf = getWatchBuff(ed);
    ed.switchToBuff("*watch");
    auto* mode = buf.getMode<watch::WatchMode>("watch");
    CMBAR_MSG(ed, "Waiting for previously running command to complete...\n");
    mode->stop();
    CMBAR_MSG(ed, "Watch command stopped successfully\n");
  },
  DEF_HELP() { return "Stops the underlying watch command."; });

DEF_CMD(
  WatchRestart, "watch::restart", DEF_OP() {
    auto& buf = getWatchBuff(ed);
    ed.switchToBuff("*watch");
    auto* mode = buf.getMode<watch::WatchMode>("watch");
    CMBAR_MSG(ed, "Restarting the watch command...\n");
    mode->restart();
    CMBAR_MSG(ed, "Watch command restarted successfully\n");
  },
  DEF_HELP() { return "Restarts the underlying watch command."; });

} // end namespace ops
} // end namespace watch
} // end namespace teditor
