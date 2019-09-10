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
  void showTopAccounts(Buffer& buf) const;
  void showAllAccounts(Buffer& buf) const;

private:
  Parser p;

  void printHeader(Buffer& buf) const;
};

} // end namespace ledger
} // end namespace teditor
