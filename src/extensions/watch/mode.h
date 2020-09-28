#pragma once

#include "../base/readonly.h"
#include "core/buffer.h"
#include <atomic>
#include <memory>
#include <thread>
#include "core/editor.h"

namespace teditor {

class Editor;

namespace watch {

/** watch mode */
class WatchMode: public readonly::ReadOnlyMode {
 public:
  WatchMode();
  ~WatchMode() { stop(); }

  Strings cmdNames() const;

  static Mode* create() { return new WatchMode; }
  static bool modeCheck(const std::string& file) { return file == "*watch"; }

  void start(Editor* ed, Buffer* buf, const std::string& cmd,
             int sleepLenMs = DefaultSleepMs);
  void stop();
  void restart() { stop(); start(); }

  int sleepTimeMs() const { return sleepMilliSec; }

 private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };

  static const int DefaultSleepMs;

  Editor* editor;  // NOT owned by this class
  Buffer* buf;  // NOT owned by this class
  std::string watchCmd;
  int sleepMilliSec;
  std::atomic<bool> alreadyRunning;
  std::shared_ptr<std::thread> runner;

  void start();
  void writeOutput();
};  // class WatchMode

}  // namespace watch
}  // namespace teditor
