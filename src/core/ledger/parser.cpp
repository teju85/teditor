#include "parser.h"

namespace teditor {
namespace ledger {

Parser::Parser(const Buffer& b):
  trans(), accts(),
  accState(), accRx("^account\\s+(\\S+)"), accDescRx("^  description\\s+(.*)"),
  accAliasRx("^  alias\\s+(\\S+)"),
  traRx("^(\\d+/\\d+/\\d+)\\s+(.*)"),
  traOpRx("^  (\\S+)\\s+(-?\\d+[.]?(\\d+)?)"), traOpDefRx("^  (\\S+)$") {
  accState.clear();
  parse(b);
}

void Parser::parse(const Buffer& b) {
  int len = b.length();
  for(int line=0;line<len;++line) {
    const auto& str = b.at(line).get();
    // empty line or comments
    if(str.empty() || str[0] == '#') continue;
    // account definition
    auto mAcc = accRx.find(str);
    if(!mAcc.empty()) {
      accState.name = mAcc.get(1);
      while(true) {
        ++line;
        const auto& s = b.at(line).get();
        auto m1 = accDescRx.find(s);
        auto m2 = accAliasRx.find(s);
        if(!m1.empty()) {
          accState.desc = m1.get(1);
        } else if(!m2.empty()) {
          accState.als.insert(m2.get(1));
        } else {
          --line;
          accts.push_back(Account(accState.name, accState.desc, accState.als));
          accState.clear();
          break;
        }
      }
      continue;
    } // end account definition
    // transactions
    auto mTra = traRx.find(str);
    if(!mTra.empty()) {
      Transaction tr(mTra.get(1), mTra.get(2));
      while(true) {
        ++line;
        const auto& s = b.at(line).get();
        auto m1 = traOpRx.find(s);
        auto m2 = traOpDefRx.find(s);
        if(!m1.empty()) {
          tr.add(m1.get(1), str2double(m1.get(2)));
        } else if(!m2.empty()) {
          tr.add(m2.get(1));
        } else {
          --line;
          trans.push_back(tr);
          break;
        }
      }
    } // end transactions
  }
  for(auto& t : trans) t.updateAccounts(accts);
}

} // end namespace ledger
} // end namespace teditor
