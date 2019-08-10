#include "evaluate.h"
#include "core/utils.h"

namespace teditor {
namespace ledger {

Accounts Evaluate::topAccounts() const {
  Accounts topAccts;
  for(const auto& a : p.accounts()) {
    auto tokens = split(a.name(), ':');
    topAccts.find(tokens[0]) += a.rawBalance();
  }
  return topAccts;
}

void Evaluate::showTopAccounts(Buffer& buf) const {
  buf.clear();
  buf.insert("\n\n"
             "############################################################\n"
             "            Welcome to your personal ledger!\n"
             "############################################################\n"
             "\n"
             "###### Top-level accounts #######\n");
  auto top = topAccounts();
  for(const auto& t : top)
    buf.insert(format("%16s    %8.2lf\n", t.name().c_str(), t.balance()));
  buf.insert("\n");
}

} // end namespace ledger
} // end namespace teditor
