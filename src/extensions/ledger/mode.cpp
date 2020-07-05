#include "mode.h"
#include "../base/text.h"
#include "core/option.h"
#include "core/parser/nfa.h"

namespace teditor {
namespace ledger {

/** ledger file edit mode */
class LedgerMode : public text::TextMode {
 public:
  LedgerMode(): text::TextMode("ledger") {}

  Strings cmdNames() const {
    return allCmdNames([](const std::string& name) {
      return name[0] != '.' && name.find("ledger::") == 0;
    });
  }

  static Mode* create() { return new LedgerMode; }

  static bool modeCheck(const std::string& file) {
    static parser::NFA nfa(".*[.]lg");
    return nfa.find(file) != parser::NFA::NoMatch;
  }
};  // class LedgerMode

REGISTER_MODE(LedgerMode, "ledger");

LedgerShowMode::LedgerShowMode(): readonly::ReadOnlyMode("ledger-show") {
  populateKeyMap<LedgerShowMode::Keys>(getKeyCmdMap());
  populateColorMap<LedgerShowMode::Colors>(getColorMap());
}

void LedgerShowMode::showTopAccounts(Buffer& buf,
                                     const std::string& file) const {
  Parser p(file);
  printHeader(buf, p);
  buf.insert("### Top-level accounts ###\n");
  auto top = p.topAccounts();
  double total = 0.0;
  for (const auto& t : top) {
    total += t.balance();
    auto valStr = format("%.2lf", t.balance());
    buf.insert(format("%12s  %-16s\n", valStr.c_str(), t.name().c_str()));
  }
  buf.insert("------------------------------\n");
  auto valStr = format("%.2lf", total);
  buf.insert(format("%12s  %-16s\n", valStr.c_str(), "Total"));
}

void LedgerShowMode::showAllAccounts(Buffer& buf,
                                     const std::string& file) const {
  Parser p(file);
  printHeader(buf, p);
  printHeader(buf, p);
  buf.insert("### All accounts ###\n");
  auto all = p.allAccounts();
  double total = 0.0;
  for (const auto& a : all) {
    const auto& name = a.name();
    if(name[0] != ' ' || name[1] != ' ') total += a.balance();
    auto valStr = format("%.2lf", a.balance());
    buf.insert(format("%12s  %-16s\n", valStr.c_str(), a.name().c_str()));
  }
  buf.insert("------------------------------\n");
  auto valStr = format("%.2lf", total);
  buf.insert(format("%12s  %-16s\n", valStr.c_str(), "total"));
}

void LedgerShowMode::printHeader(Buffer& buf, Parser& p) const {
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

REGISTER_MODE(LedgerShowMode, "ledger-show");

std::vector<KeyCmdPair> LedgerShowMode::Keys::All = {};

std::vector<NameColorPair> LedgerShowMode::Colors::All = {};

}  // end namespace ledger
}  // end namespace teditor
