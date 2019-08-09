#pragma once

#include "objects.h"
#include "core/buffer.h"
#include "core/pcre.h"

namespace teditor {
namespace ledger {

/** Helper class to parse the ledger from a buffer */
class Parser {
public:
  Parser(const Buffer& b);
  ~Parser();

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

  void parse(const Buffer& b);
};

} // end namespace ledger
} // end namespace teditor
