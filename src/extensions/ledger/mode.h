#pragma once

#include "../base/readonly.h"
#include "core/buffer.h"
#include "parser.h"

namespace teditor {
namespace ledger {

/** ledger mode */
class LedgerMode: public readonly::ReadOnlyMode {
 public:
  LedgerMode();

  static Mode* create() { return new LedgerMode; }
  static bool modeCheck(const std::string& file) { return file == "*ledger"; }
  void showTopAccounts(Buffer& buf) const;
  void showAllAccounts(Buffer& buf) const;

 private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };

  std::string ledgerFile;

  void printHeader(Buffer& buf, Parser& p) const;
};  // class LedgerMode

}  // namespace ledger
}  // namespace teditor
