#include "core/editor.h"
#include "core/command.h"
#include "core/isearch.h"
#include "core/ledger/evaluate.h"


namespace teditor {
namespace ledger {
namespace ops {

DEF_CMD(
  Ledger, "ledger", DEF_OP() {
    auto& buf = ed.getLedgerBuff();
    Buffer tmp;
    tmp.load(ed.getArgs().getLedgerFile());
    Evaluate eval(tmp);
    eval.showTopAccounts(buf);
    ed.switchToBuff("*ledger");
  },
  DEF_HELP() { return "Starts ledger-mode buffer, if not already done."; });

} // end namespace ops
} // end namespace ledger
} // end namespace teditor
