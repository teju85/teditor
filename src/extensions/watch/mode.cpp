#include "mode.h"
#include "core/option.h"

namespace teditor {
namespace watch {

const int WatchMode::DefaultSleepMs = 100;

WatchMode::WatchMode(): readonly::ReadOnlyMode("watch"), watchCmd(),
                        sleepMilliSec(DefaultSleepMs), alreadyRunning(false) {
  populateKeyMap<WatchMode::Keys>(getKeyCmdMap());
  populateColorMap<WatchMode::Colors>(getColorMap());
}

void WatchMode::start(const std::string& cmd, int sleepLenMs) {
  if (cmd.empty() || alreadyRunning) return;
  watchCmd = cmd;
  ///@todo implement
  alreadyRunning = true;
  sleepMilliSec = sleepLenMs;
}

void WatchMode::restart() {
  if (watchCmd.empty() || alreadyRunning) return;
  ///@todo implement
}

void WatchMode::stop() {
  if (!alreadyRunning) return;
  ///@todo implement
  alreadyRunning = false;
  watchCmd.clear();
}

REGISTER_MODE(WatchMode, "watch");


std::vector<KeyCmdPair> WatchMode::Keys::All = {
  {"q", "watch-stop"},
  {"r", "watch-restart"},
};

std::vector<NameColorPair> WatchMode::Colors::All = {};

}  // end namespace watch
}  // end namespace teditor
