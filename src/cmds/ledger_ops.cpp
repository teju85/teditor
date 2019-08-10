#include "core/editor.h"
#include "core/command.h"
#include "core/isearch.h"


namespace teditor {
namespace LedgerOps {

DEF_CMD(
  Ledger, "ledger", DEF_OP() {
    ///@todo
    //auto& buf = ed.getLedgerBuff();
  },
  DEF_HELP() { return "Starts ledger-mode buffer, if not already done."; });

DEF_CMD(
  Reload, "ledger-reload", DEF_OP() {
    ///@todo
    //auto& buf = ed.getLedgerBuff();
  },
  DEF_HELP() { return "Recomputes ledger data and refreshes the buffer."; });

} // end namespace LedgerOps
} // end namespace teditor
