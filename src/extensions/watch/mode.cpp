#include "mode.h"
#include "core/option.h"
#include "core/utils.h"
#include <chrono>
#include <ctime>
#include "core/editor.h"

namespace teditor {
namespace watch {

const int WatchMode::DefaultSleepMs = 1000;

WatchMode::WatchMode():
  readonly::ReadOnlyMode("watch"), editor(nullptr), buf(nullptr), watchCmd(),
  sleepMilliSec(DefaultSleepMs), alreadyRunning(false), runner() {
  populateKeyMap<WatchMode::Keys>(getKeyCmdMap());
  populateColorMap<WatchMode::Colors>(getColorMap());
}

Strings WatchMode::cmdNames() const {
  return allCmdNames([](const std::string& name) {
    return name[0] != '.' && name.find("watch::") == 0;
  });
}

void WatchMode::start() {
  if (watchCmd.empty() || alreadyRunning) return;
  alreadyRunning = true;
  auto lambda = [&]() {
    while (this->alreadyRunning) {
      this->writeOutput();
      this->editor->refresh();
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepMilliSec));
    }
  };
  runner.reset(new std::thread(lambda));
}

void WatchMode::start(Editor* ed, Buffer* b, const std::string& cmd, int sleepLenMs) {
  if (cmd.empty()) return;
  if (alreadyRunning) stop();
  editor = ed;
  buf = b;
  watchCmd = cmd;
  sleepMilliSec = sleepLenMs;
  start();
}

void WatchMode::stop() {
  if (!alreadyRunning) return;
  alreadyRunning = false;
  runner->join();
}

void WatchMode::writeOutput() {
  auto res = check_output(watchCmd);
  char timeStr[256] = {0};
  auto now = std::chrono::system_clock::now();
  auto asTime = std::chrono::system_clock::to_time_t(now);
  std::strftime(timeStr, sizeof(timeStr), "Time   : %Y-%m-%d %H:%M:%S",
                std::localtime(&asTime));
  buf->clear();
  auto str = format("Cmd     : %s\nRefresh : %d ms\n%s\n\n", watchCmd.c_str(),
                    sleepMilliSec, timeStr);
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
