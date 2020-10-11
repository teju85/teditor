#include <algorithm>
#include <fstream>
#include <map>
#include "parser.h"
#include <vector>
#include "core/parser/regexs.h"
#include "core/parser/lexer.h"
#include "core/parser/scanner.h"
#include "core/buffer.h"

namespace teditor {
namespace ledger {

enum Tokens {
  Comment = 0,
  Name,
  Date,
  Number,
  Newline,
  Space,
  AccountStart,
  AccountDescription,
  AccountAlias,
};  // enum Tokens

parser::Lexer& getLexer() {
  static parser::Lexer lexer(
    {
      {Comment, "# [^\r\n]+", "Coment"},
      {Name, "[^ \t\r\n#]+", "Name"},
      {Date, parser::Regexs::DateTime, "Date"},
      {Number, parser::Regexs::FloatingPt, "Number"},
      {Newline, parser::Regexs::Newline, "Newline"},
      {Space, "\\s+", "Space"},
      {AccountStart, "account\\s+", "AccountStart"},
      {AccountDescription, "  description\\s+", "AccountDescription"},
      {AccountAlias, "  alias\\s+", "AccountAlias"},
    });
  return lexer;
}

Parser::Parser(const std::string& f): file(f), trans(), accts() {
  parse(file);
}

void Parser::parse(const std::string& f) {
  auto tmp = isAbs(f) ? f : rel2abs(getpwd(), f);
  Buffer buff;
  buff.load(tmp, 0);
  parser::BufferScanner scanner(buff);
  auto& lexer = getLexer();
  std::unordered_set<uint32_t> ignores{Space, Newline, Comment};
  parser::Token token;
  // to get the matched string
  auto getString = [&]() -> std::string {
    return scanner.at(token.start, token.end);
  };
  // to collect a full sentence from the current line
  auto collectSentence = [&]() -> std::string {
    std::string ret;
    do {
      token = lexer.next(&scanner, Space);
      if (token.type == Name) ret += " " + getString();
    } while (token.type == Name);
    ASSERT(token.type == Newline,
           "Sentence must end with a newline '%s'. Found token=%s",
           ret.c_str(), lexer.token2str(token.type));
    return ret;
  };
  // reads next token
  auto next = [&]() { token = lexer.next(&scanner, ignores); };
  // main parser loop
  next();
  while (!token.isEof()) {
    if (token.type == AccountStart) {  // account info
      // name
      next();
      ASSERT(token.type == Name, "Invalid token after 'account'");
      auto accName = getString();
      // description
      next();
      ASSERT(token.type == AccountDescription,
             "Second line of account '%s' must be its description",
             accName.c_str());
      auto accDesc = collectSentence();
      next();
      ASSERT(token.type == AccountAlias,
             "Account '%s' description must be followed by list of its aliases",
             accName.c_str());
      // alias
      Aliases accAls;
      do {
        next();
        ASSERT(token.type == Name,
               "Account '%s' alias keyword must be followed by alias name",
               accName.c_str());
        accAls.insert(getString());
      } while (token.type == AccountAlias);
      accts.push_back(Account(accName, accDesc, accAls));
    } else if (token.type == Date) {  // transaction info
      auto dateStr = getString();
      // description
      auto desc = collectSentence();
      next();
      Transaction tr(dateStr, desc);
      ASSERT(token.type == Name,
             "Transaction start of '%s' must be followed by account name",
             desc.c_str());
      // accounts
      while (token.type == Name) {
        auto acc = getString();
        next();
        if (token.type == Number) {
          tr.add(acc, str2double(getString()));
          next();
        } else {
          tr.add(acc);
        }
      }
      trans.push_back(tr);
    } else {
      next();
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

void Parser::minmaxDates(TimePoint& min, TimePoint& max) const {
  bool first = true;
  for(const auto& t : trans) {
    if(first || t.date() < min) min = t.date();
    if(first || max < t.date()) max = t.date();
    first = false;
  }
}

} // end namespace ledger
} // end namespace teditor
