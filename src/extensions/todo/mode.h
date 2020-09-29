#pragma once

#include "../base/readonly.h"
#include "core/buffer.h"

namespace teditor {
namespace todo {

/** todo show mode */
class TodoShowMode: public readonly::ReadOnlyMode {
 public:
  TodoShowMode();

  static Mode* create() { return new TodoShowMode; }
  static bool modeCheck(const std::string& file) { return file == "*todo"; }

 private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };
};  // class TodoShowMode

}  // namespace todo
}  // namespace teditor
