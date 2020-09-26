#include <algorithm>
#include <fstream>
#include <map>
#include "parser.h"
#include <vector>
#include "core/parser/grammar.h"
#include "core/parser/regexs.h"
#include "core/parser/lexer.h"
#include "core/parser/scanner.h"
#include "core/buffer.h"
#include <iostream>

namespace teditor {
namespace ledger {

std::vector<parser::Grammar::TerminalDef>& getTokens() {
  static std::vector<parser::Grammar::TerminalDef> tokens = {
    {"Comment", "# [^\r\n]+"},
    {"Name", "[^ \t\r\n#]+"},
    {"Date", "\\d\\d\\d\\d/\\d\\d?/\\d\\d?"},  // YYYY/MM/DD or YYYY/M/D
    {"Number", parser::Regexs::FloatingPt},
    {"Newline", parser::Regexs::Newline},
    {"Space", "\\s+"},
    {"AccountStart", "account\\s+"},
    {"AccountDescription", "  description\\s+"},
    {"AccountAlias", "  alias\\s+"},
  };
  return tokens;
}

parser::Grammar& getGrammar() {
  static parser::Grammar grammar(getTokens(), {}, "");
  return grammar;
}

Parser::Parser(const std::string& f):
  file(f), trans(), accts() {
  parse(file);
}

void Parser::parse(const std::string& f) {
  auto tmp = isAbs(f) ? f : rel2abs(getpwd(), f);
  Buffer buff;
  buff.load(tmp, 0);
  parser::BufferScanner scanner(buff);
  auto& g = getGrammar();
  auto lexer = g.getLexer();
  std::unordered_set<uint32_t> ignores{
    g.getId("Space"), g.getId("Newline"), g.getId("Comment")};
  parser::Token token;
  auto collectSentence = [&]() -> std::string {
    auto space = g.getId("Space");
    std::string ret;
    do {
      token = lexer->nextWithIgnore(&scanner, space);
      if (token.type == g.getId("Name")) {
        ret += " " + scanner.at(token.start, token.end);
      }
    } while (token.type == g.getId("Name"));
    ASSERT(token.type == g.getId("Newline"),
           "Sentence must end with a newline '%s'. Found token=%s",
           ret.c_str(), g.getName(token.type).c_str());
    return ret;
  };
  // main parser loop
  token = lexer->nextWithIgnore(&scanner, ignores);
  while (!token.isEof()) {
    if (token.type == g.getId("AccountStart")) {  // account info
      // name
      token = lexer->nextWithIgnore(&scanner, ignores);
      ASSERT(token.type == g.getId("Name"), "Invalid token after 'account'");
      auto accName = scanner.at(token.start, token.end);
      // description
      token = lexer->nextWithIgnore(&scanner, ignores);
      ASSERT(token.type == g.getId("AccountDescription"),
             "Second line of account '%s' must be its description",
             accName.c_str());
      auto accDesc = collectSentence();
      token = lexer->nextWithIgnore(&scanner, ignores);
      ASSERT(token.type == g.getId("AccountAlias"),
             "Account '%s' description must be followed by list of its aliases",
             accName.c_str());
      // alias
      Aliases accAls;
      do {
        token = lexer->nextWithIgnore(&scanner, ignores);
        ASSERT(token.type == g.getId("Name"),
               "Account '%s' alias keyword must be followed by alias name",
               accName.c_str());
        accAls.insert(scanner.at(token.start, token.end));
      } while (token.type == g.getId("AccountAlias"));
      accts.push_back(Account(accName, accDesc, accAls));
    } else if (token.type == g.getId("Date")) {  // transaction info
      auto dateStr = scanner.at(token.start, token.end);
      // description
      auto desc = collectSentence();
      token = lexer->nextWithIgnore(&scanner, ignores);
      Transaction tr(dateStr, desc);
      ASSERT(token.type == g.getId("Name"),
             "Transaction start of '%s' must be followed by account name",
             desc.c_str());
      // accounts
      while (token.type == g.getId("Name")) {
        auto acc = scanner.at(token.start, token.end);
        token = lexer->nextWithIgnore(&scanner, ignores);
        if (token.type == g.getId("Number")) {
          tr.add(acc, str2double(scanner.at(token.start, token.end)));
          token = lexer->nextWithIgnore(&scanner, ignores);
        } else {
          tr.add(acc);
        }
      }
      trans.push_back(tr);
    } else {
      token = lexer->nextWithIgnore(&scanner, ignores);
    }
  }  // while
  for(auto& t : trans) t.updateAccounts(accts);
}

void Parser::reload() {
  trans.clear();
  accts.clear();
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
  for(int i=0;i<(int)accts.size();++i) {
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
