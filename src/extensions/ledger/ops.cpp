#include "core/editor.h"
#include "core/command.h"
#include "core/option.h"
#include "parser.h"

namespace teditor {
namespace ledger {
namespace ops {

Buffer& getLedgerShowBuff(Editor& ed) {
  bool newOne;
  auto& buf = ed.getBuff("*ledger", true, newOne);
  if (newOne)
    buf.setMode(Mode::createMode(Mode::inferMode(buf.bufferName(), false)));
  return buf;
}

DEF_CMD(
  LedgerOpen, "ledger", DEF_OP() {
    auto ledgerFile = Option::get("ledger::file").getStr();
    ed.load(ledgerFile, 0);
  },
  DEF_HELP() { return "Opens the ledger file in a new buffer."; });

void printHeader(Buffer& buf, const Date& min, const Date& max) {
  buf.clear();
  buf.insert("############################################################\n"
             "            Welcome to your personal ledger!\n"
             "############################################################\n"
             "\n");
  auto s = format("### Dates: %d/%d/%d to %d/%d/%d ###\n\n",
                  min.year, min.month, min.day, max.year, max.month, max.day);
  buf.insert(s);
}

void showTopAccounts(Buffer& buf) {
  auto file = Option::get("ledger::file").getStr();
  Parser p(file);
  Date min("0/0/0"), max("0/0/0");
  p.minmaxDates(min, max);
  printHeader(buf, min, max);
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

DEF_CMD(
  LedgerTop, "ledger::top", DEF_OP() {
    auto& buf = getLedgerShowBuff(ed);
    showTopAccounts(buf);
    ed.switchToBuff("*ledger");
  },
  DEF_HELP() { return "Starts ledger-mode buffer, if not already done."; });

void showAllAccounts(Buffer& buf) {
  auto file = Option::get("ledger::file").getStr();
  Parser p(file);
  Date min("0/0/0"), max("0/0/0");
  p.minmaxDates(min, max);
  printHeader(buf, min, max);
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

DEF_CMD(
  Ledger, "ledger::all", DEF_OP() {
    auto& buf = getLedgerShowBuff(ed);
    showAllAccounts(buf);
    ed.switchToBuff("*ledger");
  },
  DEF_HELP() { return "Shows all account transaction summary."; });

} // end namespace ops
} // end namespace ledger
} // end namespace teditor
