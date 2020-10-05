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
#include "core/time_utils.h"

namespace teditor {
namespace todo {

std::vector<parser::Grammar::TerminalDef>& getTokens() {
  static std::vector<parser::Grammar::TerminalDef> tokens = {
    {"Comment", "# [^\r\n]+"},
    // different keywords
    {"Keywords", "(on|for|repeat|until)"},
    // YYYY-MM-DD HH:MM:SS
    {"Date", "\\d\\d\\d\\d-\\d\\d-\\d\\d( \\d\\d:\\d\\d:\\d\\d)?"},
    // string
    {"String", "\"[^\"]*\""},
    // repetition type
    {"RepeatType", "(yearly|monthly|weekly|daily)"},
    {"Newline", parser::Regexs::Newline},
    {"Space", "\\s+"},
  };
  return tokens;
}

parser::Grammar& getGrammar() {
  static parser::Grammar grammar(getTokens(), {}, "");
  return grammar;
}

void Parser::parse(const std::string& f) {
  auto tmp = isAbs(f) ? f : rel2abs(getpwd(), f);
  Buffer buff;
  buff.load(tmp, 0);
  parser::BufferScanner scanner(buff);
  auto& g = getGrammar();
  auto lexer = g.getLexer();
  auto id_Comment = g.getId("Comment"), id_Keywords = g.getId("Keywords"),
    id_Date = g.getId("Date"), id_String = g.getId("String"),
    id_RepeatType = g.getId("RepeatType"), id_Newline = g.getId("Newline"),
    id_Space = g.getId("Space");
  // reads next token
  parser::Token token;
  auto next = [&]() { token = lexer->nextWithIgnore(&scanner, id_Space); };
  // to get the matched string
  auto getString = [&]() -> std::string {
    return scanner.at(token.start, token.end);
  };
  curr.clear();
  items_.clear();
  next();
  while (!token.isEof()) {
    if (token.type == id_Comment) {
      next();
      continue;
    }
    if (token.type == id_Newline) {
      if (curr.hasStart) {
        items_.push_back(curr);
      }
      curr.clear();
      next();
      continue;
    }
    ASSERT(token.type == id_Keywords, "Expected 'Keywords' token, found '%s'",
           g.getName(token.type).c_str());
    auto keyword = getString();
    next();
    auto value = getString();
    if (keyword == "on") {  // on <date>
      ASSERT(token.type == id_Date, "Expected 'Date' token, found '%s'",
             g.getName(token.type).c_str());
      curr.hasStart = true;
      curr.start = timeFromStr(value);
    } else if (keyword == "for") {  // for "description"
      ASSERT(token.type == id_String, "Expected 'String' token, found '%s'",
             g.getName(token.type).c_str());
      curr.description = value;
    } else if (keyword == "repeat") {  // repeat <repeatType>
      ASSERT(token.type == id_RepeatType,
             "Expected 'RepeatType' token, found '%s'",
             g.getName(token.type).c_str());
      curr.repeat = strToRepeatType(value);
    } else if (keyword == "until") {  // until <date>
      ASSERT(token.type == id_Date, "Expected 'Date' token, found '%s'",
             g.getName(token.type).c_str());
      curr.hasEnd = true;
      curr.end = timeFromStr(value);
    } else {
      ASSERT(false, "Invalid keyword found '%s'", keyword.c_str());
    }
    next();
  }
  if (curr.hasStart) items_.push_back(curr);
  curr.clear();
}

} // end namespace todo
} // end namespace teditor
