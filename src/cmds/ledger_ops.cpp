#include "core/editor.h"
#include "core/command.h"
#include "core/isearch.h"
#include "core/ledger/evaluate.h"


namespace teditor {
namespace ledger {
namespace ops {

DEF_CMD(
  Ledger, "ledger", DEF_OP() {
    ed.getLedgerBuff();
  },
  DEF_HELP() { return "Starts ledger-mode buffer, if not already done."; });

DEF_CMD(
  Reload, "ledger-reload", DEF_OP() {
    auto& buf = ed.getLedgerBuff();
    buf.clear();
  },
  DEF_HELP() { return "Recomputes ledger data and refreshes the buffer."; });

} // end namespace ops
} // end namespace ledger
} // end namespace teditor
