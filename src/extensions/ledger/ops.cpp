#include "core/editor.h"
#include "core/command.h"
#include "mode.h"

namespace teditor {
namespace ledger {
namespace ops {

Buffer& getLedgerShowBuff(Editor& ed) {
  bool newOne;
  auto& buf = ed.getBuff("*ledger", true, newOne);
  if (newOne)
    buf.setMode(Mode::createMode(Mode::inferMode(buf.bufferName(), false)));
  return buf;
}

DEF_CMD(
  LedgerTop, "ledger::top", DEF_OP() {
    auto& currBuf = ed.getBuff();
    if (currBuf.modeName() != "ledger") {
      CMBAR_MSG(ed, "Current buffer doesn't have a ledger file in it!\n");
      return;
    }
    auto& buf = getLedgerShowBuff(ed);
    auto* mode = buf.getMode<ledger::LedgerShowMode>("ledger-show");
    mode->showTopAccounts(buf, currBuf.getFileName());
    ed.switchToBuff("*ledger");
  },
  DEF_HELP() { return "Starts ledger-mode buffer, if not already done."; });

DEF_CMD(
  Ledger, "ledger::all", DEF_OP() {
    auto& currBuf = ed.getBuff();
    if (currBuf.modeName() != "ledger") {
      CMBAR_MSG(ed, "Current buffer doesn't have a ledger file in it!\n");
      return;
    }
    auto& buf = getLedgerShowBuff(ed);
    auto* mode = buf.getMode<ledger::LedgerShowMode>("ledger-show");
    mode->showAllAccounts(buf, currBuf.getFileName());
    ed.switchToBuff("*ledger");
  },
  DEF_HELP() { return "Shows all account transaction summary."; });

} // end namespace ops
} // end namespace ledger
} // end namespace teditor
