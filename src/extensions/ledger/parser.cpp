#include <algorithm>
#include <fstream>
#include <map>
#include "parser.h"

namespace teditor {
namespace ledger {

Parser::Parser(const std::string& f):
  file(f), trans(), accts(),
  accState(), accRx("^account\\s+(\\S+)"), accDescRx("^  description\\s+(.*)"),
  accAliasRx("^  alias\\s+(\\S+)"),
  traRx("^(\\d+/\\d+/\\d+)\\s+(.*)"),
  traOpRx("^  (\\S+)\\s+(-?\\d+[.]?(\\d+)?)"), traOpDefRx("^  (\\S+)$") {
  accState.clear();
  parse(file);
}

void Parser::parse(const std::string& f) {
  std::fstream fp;
  fp.open(f.c_str(), std::fstream::in);
  ASSERT(fp.is_open(), "Failed to read ledger file '%s'", f.c_str());
  std::string str;
  size_t line;
  while(std::getline(fp, str)) {
    // empty line or comments
    if(str.empty() || str[0] == '#') continue;
    // account definition
    auto mAcc = accRx.find(str);
    if(!mAcc.empty()) {
      accState.name = mAcc.get(1);
      while(true) {
        ++line;
        std::string s;
        std::getline(fp, s);
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
        std::string s;
        std::getline(fp, s);
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
  } // end while
  for(auto& t : trans) t.updateAccounts(accts);
}

void Parser::reload() {
  trans.clear();
  accts.clear();
  accState.clear();
  parse(file);
}

Accounts Parser::topAccounts(bool sort) const {
  Accounts topAccts;
  for(const auto& a : accts) {
    auto tokens = split(a.name(), ':');
    topAccts.find(tokens[0]) += a.rawBalance();
  }
  if (sort) {
    std::sort(topAccts.begin(), topAccts.end(),
              [](const Account& a, const Account& b) {
                return a.name().compare(b.name()) <= 0;
              });
  }
  return topAccts;
}

Accounts Parser::allAccounts(bool sort) const {
  std::map<std::string, Accounts> tmp;
  Accounts all;
  for (size_t i = 0; i < accts.size(); ++i) {
    const auto& a = accts[i];
    auto tokens = split(a.name(), ':');
    if(tmp.find(tokens[0]) == tmp.end()) tmp[tokens[0]] = Accounts();
    auto& currAccts = tmp[tokens[0]];
    currAccts.find(tokens[0]) += a.rawBalance();
    auto joined = "  " + join(tokens, ':', 1);
    currAccts.find(joined) += a.rawBalance();
  }
  for(auto& itr : tmp) {
    if (sort) {
      std::sort(itr.second.begin(), itr.second.end(),
                [](const Account& a, const Account& b) {
                  const auto& an = a.name();
                  const auto& bn = b.name();
                  if (an[0] == ' ' && bn[0] != ' ') return false;
                  if (an[0] != ' ' && bn[0] == ' ') return true;
                  return an.compare(bn) <= 0;
                });
    }
    for(const auto& a : itr.second) all.find(a.name()) += a.rawBalance();
  }
  return all;
}

void Parser::minmaxDates(Date& min, Date& max) const {
  bool first = true;
  for(const auto& t : trans) {
    if(first || t.date() < min) min = t.date();
    if(first || max < t.date()) max = t.date();
    first = false;
  }
}

} // end namespace ledger
} // end namespace teditor
