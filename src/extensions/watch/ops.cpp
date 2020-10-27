#include "core/editor.h"
#include "core/command.h"
#include "mode.h"
#include "core/utils.h"

namespace teditor {
namespace watch {
namespace ops {

/**
 * @page watch_ops Operations under watch-mode
 * All operations supported under `watch-mode`.
 *
 * @tableofcontents
 */

Buffer& getWatchBuff(Editor& ed) {
  bool newOne;
  auto& buf = ed.getBuff("*watch", true, newOne);
  if (newOne) {
    buf.setMode(Mode::createMode(Mode::inferMode(buf.bufferName(), false)));
  }
  return buf;
}

/**
 * @page watch_ops
 * @section watch
 * Prompts for a command and start a watch on it. Sleep time between successive
 * invocations of this shell command is controlled by the Option
 * `watch:defaultSleepMs`. The `stdout` and `stderr` of the command will be
 * printed in the current Buffer.
 *
 * @note Available since v1.8.0.
 */
DEF_CMD(
  Watch, "watch", DEF_OP() {
    auto cmd = ed.prompt("Command to watch: ");
    if (cmd.empty()) return;
    CMBAR_MSG(ed, "Starting watch on '%s'...\n", cmd.c_str());
    auto& buf = getWatchBuff(ed);
    ed.switchToBuff("*watch");
    auto* mode = buf.getMode<watch::WatchMode>("watch");
    mode->start(&buf, cmd);
  },
  DEF_HELP() { return "Start a watch command."; });

/**
 * @page watch_ops
 * @section watch-sleep
 * Prompts for a command and then a sleep time (in ms) and starts a watch on it.
 * The `stdout` and `stderr` of the command will be printed in the current
 * Buffer.
 *
 * @note Available since v1.8.0.
 */
DEF_CMD(
  WatchSleep, "watch-sleep", DEF_OP() {
    auto cmd = ed.prompt("Command to watch: ");
    if (cmd.empty()) return;
    auto sleep = ed.prompt("Sleep time (in ms) between retries: ");
    if (sleep.empty()) return;
    CMBAR_MSG(ed, "Starting watch on '%s' with sleep-time=%s ms...\n",
              cmd.c_str(), sleep.c_str());
    auto& buf = getWatchBuff(ed);
    ed.switchToBuff("*watch");
    auto* mode = buf.getMode<watch::WatchMode>("watch");
    mode->start(&buf, cmd, str2num(sleep));
  },
  DEF_HELP() { return "Start a watch command."; });

/**
 * @page watch_ops
 * @section watch_stop watch::stop
 * Stops the currently running watch command.
 *
 * @note Available since v1.8.0.
 */
DEF_CMD(
  WatchStop, "watch::stop", DEF_OP() {
    CMBAR_MSG(ed, "Waiting for previously running command to complete...\n");
    auto& buf = getWatchBuff(ed);
    ed.switchToBuff("*watch");
    auto* mode = buf.getMode<watch::WatchMode>("watch");
    mode->stop();
    CMBAR_MSG(ed, "Watch command stopped successfully\n");
  },
  DEF_HELP() { return "Stops the underlying watch command."; });

/**
 * @page watch_ops
 * @section watch_restart watch::restart
 * Restarts the previously stopped watch command.
 *
 * @note Available since v1.8.0
 */
DEF_CMD(
  WatchRestart, "watch::restart", DEF_OP() {
    CMBAR_MSG(ed, "Restarting the watch command...\n");
    auto& buf = getWatchBuff(ed);
    ed.switchToBuff("*watch");
    auto* mode = buf.getMode<watch::WatchMode>("watch");
    mode->restart();
    CMBAR_MSG(ed, "Watch command restarted successfully\n");
  },
  DEF_HELP() { return "Restarts the underlying watch command."; });

} // end namespace ops
} // end namespace watch
} // end namespace teditor
