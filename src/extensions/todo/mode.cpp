#include "mode.h"
#include "../base/text.h"
#include "core/option.h"
#include "core/parser/nfa.h"

namespace teditor {
namespace todo {

/** todo file edit mode */
class TodoMode : public text::TextMode {
 public:
  TodoMode(): text::TextMode("todo") {}

  Strings cmdNames() const {
    return allCmdNames([](const std::string& name) {
      return name[0] != '.' && name.find("todo::") == 0;
    });
  }

  static Mode* create() { return new TodoMode; }

  static bool modeCheck(const std::string& file) {
    static parser::NFA nfa(".*[.]todo");
    return nfa.find(file) != parser::NFA::NoMatch;
  }
};  // class TodoMode

REGISTER_MODE(TodoMode, "todo");

TodoShowMode::TodoShowMode(): readonly::ReadOnlyMode("todo-show") {
  populateKeyMap<TodoShowMode::Keys>(getKeyCmdMap());
  populateColorMap<TodoShowMode::Colors>(getColorMap());
}

REGISTER_MODE(TodoShowMode, "todo-show");

std::vector<KeyCmdPair> TodoShowMode::Keys::All = {};

std::vector<NameColorPair> TodoShowMode::Colors::All = {};

}  // end namespace todo
}  // end namespace teditor
