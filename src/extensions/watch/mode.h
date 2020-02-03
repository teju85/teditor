#pragma once

#include "../base/readonly.h"
#include "core/buffer.h"

namespace teditor {
namespace watch {

/** watch mode */
class WatchMode: public readonly::ReadOnlyMode {
 public:
  WatchMode();

  static Mode* create() { return new WatchMode; }
  static bool modeCheck(const std::string& file) { return file == "*watch"; }

  void start(const std::string& cmd, int sleepLenMs = DefaultSleepMs);
  void restart();
  void stop();

 private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };

  static const int DefaultSleepMs;

  std::string watchCmd;
  int sleepMilliSec;
  bool alreadyRunning;
};  // class WatchMode

}  // namespace watch
}  // namespace teditor
