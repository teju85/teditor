#include "core/editor.h"
#include "core/command.h"
#include "mode.h"
#include "core/option.h"

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
  LedgerOpen, "ledger", DEF_OP() {
    auto ledgerFile = Option::get("ledger::file").getStr();
    ed.load(ledgerFile, 0);
  },
  DEF_HELP() { return "Opens the ledger file in a new buffer."; });

DEF_CMD(
  LedgerTop, "ledger::top", DEF_OP() {
    auto& buf = getLedgerShowBuff(ed);
    auto* mode = buf.getMode<ledger::LedgerShowMode>("ledger-show");
    mode->showTopAccounts(buf);
    ed.switchToBuff("*ledger");
  },
  DEF_HELP() { return "Starts ledger-mode buffer, if not already done."; });

DEF_CMD(
  Ledger, "ledger::all", DEF_OP() {
    auto& buf = getLedgerShowBuff(ed);
    auto* mode = buf.getMode<ledger::LedgerShowMode>("ledger-show");
    mode->showAllAccounts(buf);
    ed.switchToBuff("*ledger");
  },
  DEF_HELP() { return "Shows all account transaction summary."; });

} // end namespace ops
} // end namespace ledger
} // end namespace teditor
