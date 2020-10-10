#include <algorithm>
#include <fstream>
#include <map>
#include "parser.h"
#include <vector>
#include "core/parser/regexs.h"
#include "core/parser/lexer.h"
#include "core/parser/scanner.h"
#include "core/buffer.h"
#include "core/time_utils.h"

namespace teditor {
namespace todo {

enum Tokens {
  Comment = 0,
  Keywords,
  Date,
  String,
  RepeatType,
  Newline,
  Space,
};  // enum Tokens

#define CASE(t) case t : return #t
const char* tokens2str(uint32_t tok) {
  if (tok == parser::Token::End) return "End";
  if (tok == parser::Token::Unknown) return "Unknown";
  switch (Tokens(tok)) {
    CASE(Comment);
    CASE(Keywords);
    CASE(Date);
    CASE(String);
    CASE(RepeatType);
    CASE(Newline);
    CASE(Space);
  default:
    ASSERT(false, "tokens2str: Bad token type passed '%d'!", tok);
  }
}
#undef CASE

parser::Lexer& getLexer() {
  static parser::Lexer lexer(
    {
      {Comment, "# [^\r\n]+"},
      // different keywords
      {Keywords, "(on|for|repeat|until)"},
      // YYYY-MM-DD HH:MM:SS
      {Date, parser::Regexs::DateTime},
      // string
      {String, "\"[^\"]*\""},
      // repetition type
      {RepeatType, "(yearly|monthly|weekly|daily)"},
      {Newline, parser::Regexs::Newline},
      {Space, "\\s+"},
    });
  return lexer;
}

void Parser::parse(const std::string& f) {
  auto tmp = isAbs(f) ? f : rel2abs(getpwd(), f);
  Buffer buff;
  buff.load(tmp, 0);
  parser::BufferScanner scanner(buff);
  auto& lexer = getLexer();
  // reads next token
  parser::Token token;
  auto next = [&]() { token = lexer.next(&scanner, Space); };
  // to get the matched string
  auto getString = [&]() -> std::string {
    return scanner.at(token.start, token.end);
  };
  curr.clear();
  items_.clear();
  next();
  while (!token.isEof()) {
    if (token.type == Comment) {
      next();
      continue;
    }
    if (token.type == Newline) {
      if (curr.hasStart) {
        items_.push_back(curr);
      }
      curr.clear();
      next();
      continue;
    }
    ASSERT(token.type == Keywords, "Expected 'Keywords' token, found '%s'",
           tokens2str(token.type));
    auto keyword = getString();
    next();
    auto value = getString();
    if (keyword == "on") {  // on <date>
      ASSERT(token.type == Date, "Expected 'Date' token, found '%s'",
             tokens2str(token.type));
      curr.hasStart = true;
      curr.start = timeFromStr(value);
    } else if (keyword == "for") {  // for "description"
      ASSERT(token.type == String, "Expected 'String' token, found '%s'",
             tokens2str(token.type));
      curr.description = value;
    } else if (keyword == "repeat") {  // repeat <repeatType>
      ASSERT(token.type == RepeatType,
             "Expected 'RepeatType' token, found '%s'",
             tokens2str(token.type));
      curr.repeat = strToRepeatType(value);
    } else if (keyword == "until") {  // until <date>
      ASSERT(token.type == Date, "Expected 'Date' token, found '%s'",
             tokens2str(token.type));
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
