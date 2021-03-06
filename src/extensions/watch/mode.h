#pragma once

#include "../base/readonly.h"
#include "core/buffer.h"
#include <atomic>
#include <memory>
#include <thread>

namespace teditor {
namespace watch {

/** watch mode */
class WatchMode: public readonly::ReadOnlyMode {
 public:
  WatchMode();
  ~WatchMode() { stop(); }

  Strings cmdNames() const;

  static Mode* create() { return new WatchMode; }
  static bool modeCheck(const std::string& file) { return file == "*watch"; }

  void start(Buffer* buf, const std::string& cmd,
             int sleepLenMs = 0);
  void stop();
  void restart() { stop(); start(); }

  int sleepTimeMs() const { return sleepMilliSec; }

 private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };

  Buffer* buf;  // NOT owned by this class
  std::string watchCmd;
  int defaultSleepMilliSec;
  int sleepMilliSec;
  std::atomic<bool> alreadyRunning;
  std::shared_ptr<std::thread> runner;

  void start();
  void writeOutput();
};  // class WatchMode

}  // namespace watch
}  // namespace teditor
