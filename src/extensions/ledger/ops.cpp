#include "core/editor.h"
#include "core/command.h"
#include "core/isearch.h"
#include "evaluate.h"
#include "core/option.h"

namespace teditor {
namespace ledger {
namespace ops {

Buffer& getLedgerBuff(Editor& ed) {
  bool newOne;
  auto& buf = ed.getBuff("*ledger", true, newOne);
  if (newOne) {
    buf.setMode(Mode::createMode(Mode::inferMode(buf.bufferName(), false)));
  }
  return buf;
}

DEF_CMD(
  LedgerTop, "ledger-top", DEF_OP() {
    auto& buf = getLedgerBuff(ed);
    Evaluate eval(Option::get("ledgerFile").getStr());
    eval.showTopAccounts(buf);
    ed.switchToBuff("*ledger");
  },
  DEF_HELP() { return "Starts ledger-mode buffer, if not already done."; });

DEF_CMD(
  Ledger, "ledger", DEF_OP() {
    auto& buf = getLedgerBuff(ed);
    Evaluate eval(Option::get("ledgerFile").getStr());
    eval.showAllAccounts(buf);
    ed.switchToBuff("*ledger");
  },
  DEF_HELP() { return "Shows all account transaction summary."; });

} // end namespace ops
} // end namespace ledger
} // end namespace teditor
