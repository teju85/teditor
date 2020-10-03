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

namespace teditor {
namespace todo {

std::vector<parser::Grammar::TerminalDef>& getTokens() {
  static std::vector<parser::Grammar::TerminalDef> tokens = {
    {"Comment", "# [^\r\n]+"},
    // [Name]
    {"Name", "\\[[^\\[\\]]+\\]"},
    // YYYY-MM-DD HH:MM:SS
    {"Date", "(x|(\\d\\d\\d\\d))-(x|(\\d\\d))-(x|(\\d\\d)) "
     "((x|(\\d\\d)):(x|(\\d\\d)):(x|(\\d\\d)))?"},
    // string
    {"String", "\"[^\"]*\""},
    {"Newline", parser::Regexs::Newline},
    {"Space", "\\s+"},
  };
  return tokens;
}

parser::Grammar& getGrammar() {
  static parser::Grammar grammar(getTokens(), {}, "");
  return grammar;
}

Parser::Parser(const std::string& f): file(f) {
  parse(file);
}

void Parser::parse(const std::string& f) {
  auto tmp = isAbs(f) ? f : rel2abs(getpwd(), f);
  Buffer buff;
  buff.load(tmp, 0);
  parser::BufferScanner scanner(buff);
  auto& g = getGrammar();
  auto lexer = g.getLexer();
  // auto id_Comment = g.getId("Comment"), id_Name = g.getId("Name"),
  //   id_Newline = g.getId("Newline"), id_Comment = g.getId("Comment"),
  //   id_Space = g.getId("Space"), id_Name = g.getId("Name"),
  //   id_AccountDescription = g.getId("AccountDescription"),
  //   id_AccountAlias = g.getId("AccountAlias"),
  //   id_Date = g.getId("Date"), id_Number = g.getId("Number");
  // std::unordered_set<uint32_t> ignores{id_Space, id_Newline, id_Comment};
  // parser::Token token;
  // // to get the matched string
  // auto getString = [&]() -> std::string {
  //   return scanner.at(token.start, token.end);
  // };
  // // to collect a full sentence from the current line
  // auto collectSentence = [&]() -> std::string {
  //   std::string ret;
  //   do {
  //     token = lexer->nextWithIgnore(&scanner, id_Space);
  //     if (token.type == id_Name) ret += " " + getString();
  //   } while (token.type == id_Name);
  //   ASSERT(token.type == id_Newline,
  //          "Sentence must end with a newline '%s'. Found token=%s",
  //          ret.c_str(), g.getName(token.type).c_str());
  //   return ret;
  // };
  // // reads next token
  // auto next = [&]() { token = lexer->nextWithIgnore(&scanner, ignores); };
  // // main parser loop
  // next();
  // while (!token.isEof()) {
  //   if (token.type == id_AccountStart) {  // account info
  //     // name
  //     next();
  //     ASSERT(token.type == id_Name, "Invalid token after 'account'");
  //     auto accName = getString();
  //     // description
  //     next();
  //     ASSERT(token.type == id_AccountDescription,
  //            "Second line of account '%s' must be its description",
  //            accName.c_str());
  //     auto accDesc = collectSentence();
  //     next();
  //     ASSERT(token.type == id_AccountAlias,
  //            "Account '%s' description must be followed by list of its aliases",
  //            accName.c_str());
  //     // alias
  //     Aliases accAls;
  //     do {
  //       next();
  //       ASSERT(token.type == id_Name,
  //              "Account '%s' alias keyword must be followed by alias name",
  //              accName.c_str());
  //       accAls.insert(getString());
  //     } while (token.type == id_AccountAlias);
  //     accts.push_back(Account(accName, accDesc, accAls));
  //   } else if (token.type == id_Date) {  // transaction info
  //     auto dateStr = getString();
  //     // description
  //     auto desc = collectSentence();
  //     next();
  //     Transaction tr(dateStr, desc);
  //     ASSERT(token.type == id_Name,
  //            "Transaction start of '%s' must be followed by account name",
  //            desc.c_str());
  //     // accounts
  //     while (token.type == id_Name) {
  //       auto acc = getString();
  //       next();
  //       if (token.type == id_Number) {
  //         tr.add(acc, str2double(getString()));
  //         next();
  //       } else {
  //         tr.add(acc);
  //       }
  //     }
  //     trans.push_back(tr);
  //   } else {
  //     next();
  //   }
  // }  // while
  // for(auto& t : trans) t.updateAccounts(accts);
}

} // end namespace todo
} // end namespace teditor
