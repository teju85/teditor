#include "mode.h"
#include "core/option.h"

namespace teditor {
namespace ledger {

LedgerMode::LedgerMode(): readonly::ReadOnlyMode("ledger"),
                          ledgerFile(Option::get("ledgerFile").getStr()) {
  populateKeyMap<LedgerMode::Keys>(getKeyCmdMap());
  populateColorMap<LedgerMode::Colors>(getColorMap());
}

void LedgerMode::showTopAccounts(Buffer& buf) const {
  Parser p(ledgerFile);
  printHeader(buf, p);
  buf.insert("### Top-level accounts ###\n");
  auto top = p.topAccounts();
  double total = 0.0;
  for(const auto& t : top) {
    total += t.balance();
    auto valStr = format("%.2lf", t.balance());
    buf.insert(format("%12s  %-16s\n", valStr.c_str(), t.name().c_str()));
  }
  buf.insert("------------------------------\n");
  auto valStr = format("%.2lf", total);
  buf.insert(format("%12s  %-16s\n", valStr.c_str(), "Total"));
}

void LedgerMode::showAllAccounts(Buffer& buf) const {
  Parser p(ledgerFile);
  printHeader(buf, p);
  printHeader(buf, p);
  buf.insert("### All accounts ###\n");
  auto all = p.allAccounts();
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

void LedgerMode::printHeader(Buffer& buf, Parser& p) const {
  buf.clear();
  buf.insert("############################################################\n"
             "            Welcome to your personal ledger!\n"
             "############################################################\n"
             "\n");
  Date min("0/0/0"), max("0/0/0");
  p.minmaxDates(min, max);
  auto s = format("### Dates: %d/%d/%d to %d/%d/%d ###\n\n",
                  min.year, min.month, min.day, max.year, max.month, max.day);
  buf.insert(s);
}

REGISTER_MODE(LedgerMode, "ledger");


std::vector<KeyCmdPair> LedgerMode::Keys::All = {
  {"A", "ledger"},
  {"T", "ledger-top"},
};

std::vector<NameColorPair> LedgerMode::Colors::All = {};

}  // end namespace ledger
}  // end namespace teditor
