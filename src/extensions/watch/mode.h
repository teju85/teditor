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

 private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };
};  // class WatchMode

}  // namespace watch
}  // namespace teditor
