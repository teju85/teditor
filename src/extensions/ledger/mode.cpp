#include "../base/text.h"
#include "core/option.h"
#include "core/parser/nfa.h"
#include "../base/readonly.h"
#include "core/buffer.h"

namespace teditor {
namespace ledger {

/** ledger file edit mode */
class LedgerMode : public text::TextMode {
 public:
  LedgerMode(): text::TextMode("ledger") {}

  Strings cmdNames() const {
    return allCmdNames([](const std::string& name) {
      return (name[0] != '.' && name.find("ledger::") == 0);
    });
  }

  static Mode* create() { return new LedgerMode; }

  static bool modeCheck(const std::string& file) {
    static parser::NFA nfa(".*[.]lg");
    return nfa.find(file) != parser::NFA::NoMatch;
  }
};  // class LedgerMode

REGISTER_MODE(LedgerMode, "ledger");

/** ledger show mode */
class LedgerShowMode: public readonly::ReadOnlyMode {
 public:
  LedgerShowMode(): readonly::ReadOnlyMode("ledger-show") {
    populateKeyMap<LedgerShowMode::Keys>(getKeyCmdMap());
    populateColorMap<LedgerShowMode::Colors>(getColorMap());
  }

  static Mode* create() { return new LedgerShowMode; }
  static bool modeCheck(const std::string& file) { return file == "*ledger"; }

 private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };
};  // class LedgerShowMode

std::vector<KeyCmdPair> LedgerShowMode::Keys::All = {
  {"a", "ledger::all"},
  {"t", "ledger::top"}
};

std::vector<NameColorPair> LedgerShowMode::Colors::All = {};

REGISTER_MODE(LedgerShowMode, "ledger-show");

}  // end namespace ledger
}  // end namespace teditor
