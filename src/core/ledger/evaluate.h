#pragma once

#include "objects.h"
#include "parser.h"
#include "core/buffer.h"

namespace teditor {
namespace ledger {

/** Evaluate ledger balance and registers after parsing the input file */
class Evaluate {
public:
  Evaluate(const Buffer& buf): p(buf) {}

  /**
   * @brief Computes balances of only the top-level accounts.
   * Top-level account is the account name that preceeds the first ':' in
   * all of the accounts in the ledger. Eg: top-level account of
   * 'Expenses:Medical:Vaccination' is 'Expenses'.
   */
  Accounts topAccounts() const;

  void showTopAccounts(Buffer& buf) const;

private:
  Parser p;
};

} // end namespace ledger
} // end namespace teditor
