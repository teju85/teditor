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

} // end namespace ledger
} // end namespace teditor
