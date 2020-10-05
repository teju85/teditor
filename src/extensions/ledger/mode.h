#pragma once

#include "../base/readonly.h"
#include "core/buffer.h"
#include "parser.h"

namespace teditor {
namespace ledger {

/** ledger show mode */
class LedgerShowMode: public readonly::ReadOnlyMode {
 public:
  LedgerShowMode();

  static Mode* create() { return new LedgerShowMode; }
  static bool modeCheck(const std::string& file) { return file == "*ledger"; }

  void showTopAccounts(Buffer& buf) const;
  void showAllAccounts(Buffer& buf) const;

 private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };

  void printHeader(Buffer& buf, const Date& min, const Date& max) const;
};  // class LedgerShowMode

}  // namespace ledger
}  // namespace teditor
