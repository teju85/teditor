#include "../base/readonly.h"
#include "core/buffer.h"

namespace teditor {

/** ledger mode */
class LedgerMode: public readonly::ReadOnlyMode {
public:
  LedgerMode(): readonly::ReadOnlyMode("ledger") {
    populateKeyMap<LedgerMode::Keys>(getKeyCmdMap());
    populateColorMap<LedgerMode::Colors>(getColorMap());
  }

  static Mode* create() { return new LedgerMode; }

  static bool modeCheck(const std::string& file) { return file == "*ledger"; }

private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };
};

REGISTER_MODE(LedgerMode, "ledger");


std::vector<KeyCmdPair> LedgerMode::Keys::All = {
  {"A", "ledger"},
  {"T", "ledger-top"},
};

std::vector<NameColorPair> LedgerMode::Colors::All = {};

} // end namespace teditor
