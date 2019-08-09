#pragma once

#include "objects.h"
#include "core/buffer.h"
#include "core/pcre.h"

namespace teditor {
namespace ledger {

/**
 * Poor man's ledger parser from a buffer!
 * Currently it is just a hacky collection of Pcre2 regex's
 */
class Parser {
public:
  Parser(const Buffer& b);

  const Transactions& transactions() const { return trans; }
  const Accounts& accounts() const { return accts; }

private:
  struct AccState {
    std::string name, desc;
    Aliases als;

    void clear() {
      name.clear();
      desc.clear();
      als.clear();
    }
  };

  Transactions trans;
  Accounts accts;
  AccState accState;
  Pcre accRx, accDescRx, accAliasRx;
  Pcre traRx, traOpRx, traOpDefRx;

  void parse(const Buffer& b);
};

} // end namespace ledger
} // end namespace teditor
