#include "evaluate.h"
#include "core/utils.h"
#include <set>

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

Accounts Evaluate::allAccounts() const {
  Accounts all;
  for(const auto& a : p.accounts()) {
    auto tokens = split(a.name(), ':');
    all.find(tokens[0]) += a.rawBalance();
    auto joined = "  " + join(tokens, ':', 1);
    all.find(joined) += a.rawBalance();
  }
  return all;
}

void Evaluate::minmaxDates(Date& min, Date& max) const {
  const auto& trans = p.transactions();
  bool first = true;
  for(const auto& t : trans) {
    if(first || t.date() < min) min = t.date();
    if(first || max < t.date()) max = t.date();
    first = false;
  }
}

void Evaluate::showTopAccounts(Buffer& buf) const {
  buf.clear();
  buf.insert("############################################################\n"
             "            Welcome to your personal ledger!\n"
             "############################################################\n"
             "\n");
  Date min("0/0/0"), max("0/0/0");
  minmaxDates(min, max);
  auto s = format("### Dates: %d/%d/%d to %d/%d/%d ###\n\n",
                  min.year, min.month, min.day, max.year, max.month, max.day);
  buf.insert(s);
  buf.insert("### Top-level accounts ###\n");
  auto top = topAccounts();
  double total = 0.0;
  for(const auto& t : top) {
    total += t.balance();
    auto valStr = format("%.2lf", t.balance());
    buf.insert(format("%12s  %-16s\n", valStr.c_str(), t.name().c_str()));
  }
  buf.insert("------------------------------\n");
  auto valStr = format("%.2lf", total);
  buf.insert(format("%12s  %-16s\n", valStr.c_str(), "total"));
}

void Evaluate::showAllAccounts(Buffer& buf) const {
  buf.clear();
  buf.insert("############################################################\n"
             "            Welcome to your personal ledger!\n"
             "############################################################\n"
             "\n");
  Date min("0/0/0"), max("0/0/0");
  minmaxDates(min, max);
  auto s = format("### Dates: %d/%d/%d to %d/%d/%d ###\n\n",
                  min.year, min.month, min.day, max.year, max.month, max.day);
  buf.insert(s);
  buf.insert("### All accounts ###\n");
  auto all = allAccounts();
  double total = 0.0;
  for(const auto& a : all) {
    const auto& name = a.name();
    if(name[0] != ' ' || name[1] != ' ') total += a.balance();
    auto valStr = format("%.2lf", a.balance());
    buf.insert(format("%12s  %-16s\n", valStr.c_str(), a.name().c_str()));
  }
  buf.insert("------------------------------\n");
  auto valStr = format("%.2lf", total);
  buf.insert(format("%12s  %-16s\n", valStr.c_str(), "total"));
}

} // end namespace ledger
} // end namespace teditor
