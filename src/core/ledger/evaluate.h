#pragma once

#include "objects.h"
#include "parser.h"
#include "core/buffer.h"

namespace teditor {
namespace ledger {

/** Evaluate ledger balance and registers after parsing the input file */
class Evaluate {
public:
  Evaluate(const std::string& f): p(f) {}

  /**
   * @brief Computes balances of only the top-level accounts.
   * Top-level account is the account name that preceeds the first ':' in
   * all of the accounts in the ledger. Eg: top-level account of
   * 'Expenses:Medical:Vaccination' is 'Expenses'.
   */
  Accounts topAccounts() const;

  /** Computes balances of the top-level accounts as well as all accounts */
  Accounts allAccounts() const;

  /** earliest and latest transaction dates */
  void minmaxDates(Date& min, Date& max) const;

  void showTopAccounts(Buffer& buf) const;
  void showAllAccounts(Buffer& buf) const;

private:
  Parser p;

  void printHeader(Buffer& buf) const;
};

} // end namespace ledger
} // end namespace teditor
