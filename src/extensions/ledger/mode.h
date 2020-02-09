#pragma once

#include "../base/readonly.h"
#include "../base/text.h"
#include "core/buffer.h"
#include "parser.h"

namespace teditor {
namespace ledger {

/** ledger file edit mode */
class LedgerMode : public text::TextMode {
 public:
  LedgerMode(): text::TextMode("ledger") {}

  static Mode* create() { return new LedgerMode; }
  static bool modeCheck(const std::string& file);
};  // class LedgerMode


/** ledger show mode */
class LedgerShowMode: public readonly::ReadOnlyMode {
 public:
  LedgerShowMode();

  static Mode* create() { return new LedgerMode; }
  static bool modeCheck(const std::string& file) { return file == "*ledger"; }

  void showTopAccounts(Buffer& buf) const;
  void showAllAccounts(Buffer& buf) const;

 private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };

  std::string ledgerFile;

  void printHeader(Buffer& buf, Parser& p) const;
};  // class LedgerShowMode

}  // namespace ledger
}  // namespace teditor
