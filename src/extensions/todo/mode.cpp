#include "../base/text.h"
#include "core/option.h"
#include "core/parser/nfa.h"
#include "../base/readonly.h"
#include "core/buffer.h"

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


/** todo show mode */
class TodoShowMode: public readonly::ReadOnlyMode {
 public:
  TodoShowMode(): readonly::ReadOnlyMode("todo-show") {
    populateKeyMap<TodoShowMode::Keys>(getKeyCmdMap());
    populateColorMap<TodoShowMode::Colors>(getColorMap());
  }

  static Mode* create() { return new TodoShowMode; }
  static bool modeCheck(const std::string& file) { return file == "*todo"; }

 private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };
};  // class TodoShowMode

std::vector<KeyCmdPair> TodoShowMode::Keys::All = {};

std::vector<NameColorPair> TodoShowMode::Colors::All = {};

REGISTER_MODE(TodoShowMode, "todo-show");

}  // end namespace todo
}  // end namespace teditor
