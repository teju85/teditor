#include "mode.h"
#include "core/option.h"
#include "core/utils.h"
#include <chrono>
#include <ctime>

namespace teditor {
namespace watch {

const int WatchMode::DefaultSleepMs = 100;

WatchMode::WatchMode(): readonly::ReadOnlyMode("watch"), buf(nullptr),
                        watchCmd(), sleepMilliSec(DefaultSleepMs),
                        alreadyRunning(false) {
  populateKeyMap<WatchMode::Keys>(getKeyCmdMap());
  populateColorMap<WatchMode::Colors>(getColorMap());
}

void WatchMode::start(Buffer* b, const std::string& cmd, int sleepLenMs) {
  if (cmd.empty()) return;
  ///@todo: kill any existing async's
  if (alreadyRunning) {
  }
  buf = b;
  watchCmd = cmd;
  alreadyRunning = true;
  sleepMilliSec = sleepLenMs;
  ///@todo: do this in a loop with async!
  writeOutput();
}

void WatchMode::restart() {
  if (watchCmd.empty()) return;
  ///@todo: kill any existing async's
  if (alreadyRunning) {
  }
  ///@todo: do this in a loop with async!
  writeOutput();
}

void WatchMode::stop() {
  if (!alreadyRunning) return;
  buf = nullptr;
  watchCmd.clear();
  alreadyRunning = false;
  ///@todo: kill any existing async's
}

void WatchMode::writeOutput() {
  char timeStr[256] = {0};
  auto now = std::chrono::system_clock::now();
  auto asTime = std::chrono::system_clock::to_time_t(now);
  std::strftime(timeStr, sizeof(timeStr), "Time   : %Y-%m-%d %H:%M:%S",
                std::localtime(&asTime));
  buf->clear();
  auto str = format("Cmd     : %s\nRefresh : %d ms\n%s\n\n", watchCmd.c_str(),
                    sleepMilliSec, timeStr);
  buf->insert(str);
  auto res = check_output(watchCmd);
  buf->insert("## Output\n");
  buf->insert(res.output);
  buf->insert("\n");
  buf->insert("## Error\n");
  buf->insert(res.error);
  buf->insert("\n");
}

REGISTER_MODE(WatchMode, "watch");


std::vector<KeyCmdPair> WatchMode::Keys::All = {
  {"s", "watch-stop"},
  {"r", "watch-restart"},
};

std::vector<NameColorPair> WatchMode::Colors::All = {};

}  // end namespace watch
}  // end namespace teditor
