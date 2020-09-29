#include "mode.h"
#include "core/option.h"
#include "core/utils.h"
#include <chrono>

namespace teditor {
namespace watch {

WatchMode::WatchMode():
  readonly::ReadOnlyMode("watch"), buf(nullptr), watchCmd(),
  defaultSleepMilliSec(Option::get("watch::defaultSleepMs").getInt()),
  sleepMilliSec(defaultSleepMilliSec), alreadyRunning(false), runner() {
  populateKeyMap<WatchMode::Keys>(getKeyCmdMap());
  populateColorMap<WatchMode::Colors>(getColorMap());
}

Strings WatchMode::cmdNames() const {
  return allCmdNames([](const std::string& name) {
    return (name[0] != '.' && name.find("watch::") == 0) || (name == "watch");
  });
}

void WatchMode::start() {
  if (watchCmd.empty() || alreadyRunning) return;
  alreadyRunning = true;
  auto lambda = [&]() {
    while (this->alreadyRunning) {
      this->writeOutput();
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepMilliSec));
    }
  };
  runner.reset(new std::thread(lambda));
}

void WatchMode::start(Buffer* b, const std::string& cmd, int sleepLenMs) {
  if (cmd.empty()) return;
  if (alreadyRunning) stop();
  buf = b;
  watchCmd = cmd;
  sleepMilliSec = sleepLenMs <= 0 ? defaultSleepMilliSec : sleepLenMs;
  start();
}

void WatchMode::stop() {
  if (!alreadyRunning) return;
  alreadyRunning = false;
  runner->join();
}

void WatchMode::writeOutput() {
  auto res = check_output(watchCmd);
  auto curr = getCurrentTimeAsString();
  buf->clear();
  auto str = format("Cmd     : %s\nRefresh : %d ms\nTime    : %s\n\n",
                    watchCmd.c_str(), sleepMilliSec, curr.c_str());
  buf->insert(str);
  buf->insert("## Output\n");
  buf->insert(res.output);
  buf->insert("\n");
  buf->insert("## Error\n");
  buf->insert(res.error);
  buf->insert("\n");
}

REGISTER_MODE(WatchMode, "watch");


std::vector<KeyCmdPair> WatchMode::Keys::All = {
  {"s", "watch::stop"},
  {"r", "watch::restart"},
};

std::vector<NameColorPair> WatchMode::Colors::All = {};

}  // end namespace watch
}  // end namespace teditor
