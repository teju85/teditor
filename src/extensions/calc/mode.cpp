#include "core/buffer.h"
#include "core/option.h"
#include "mode.h"
#include "core/parser/scanner.h"
#include "core/parser/regexs.h"

namespace teditor {
namespace calc {

CalcMode::CalcMode():
  text::TextMode("calc"), vars(), prompt(Option::get("calc:prompt").getStr()),
  lineSeparator(Option::get("calc:lineSeparator").getStr()),
  cmds(Option::get("calc:histFile").getStr(),
       Option::get("calc:maxHistory").getInt()), lex(nullptr) {
  populateKeyMap<CalcMode::Keys>(getKeyCmdMap());
  populateColorMap<CalcMode::Colors>(getColorMap());
  lex = new parser::Lexer({{Float,      parser::Regexs::FloatingPt},
                           {Int,        parser::Regexs::Integer},
                           {BrktOpen,   "\\("},
                           {BrktClose,  "\\)"},
                           {Comma,      ","},
                           {SemiColon,  ";"},
                           {Symbol,     parser::Regexs::Variable},
                           {Operators,  "[-+*/=]"},
                           {WhiteSpace, "\\s+"}});
}

CalcMode::~CalcMode() {
  cmds.store();
  delete lex;
}

Num64& CalcMode::getVar(const std::string& name) {
  auto itr = vars.find(name);
  ASSERT(itr != vars.end(), "variable '%s' does not exist!", name.c_str());
  return vars[name];
}

bool CalcMode::isPromptLine(Buffer& buf) const {
  const auto& pt = buf.getPoint();
  const auto& line = buf.at(pt.y).get();
  return line.compare(0, prompt.size(), prompt) == 0;
}

bool CalcMode::isOnPrompt(Buffer& buf, bool includeTrailingSpace) const {
  if (!isPromptLine(buf)) return false;
  const auto& pt = buf.getPoint();
  if (includeTrailingSpace) return pt.x <= (int)prompt.size();
  return pt.x < (int)prompt.size();
}

bool CalcMode::isOnSeparator(Buffer& buf) const {
  const auto& pt = buf.getPoint();
  const auto& line = buf.at(pt.y).get();
  return line == lineSeparator;
}

void CalcMode::printHeader(Buffer& buf) const {
  buf.end();
  if (isPromptLine(buf)) return;
  buf.insert(lineSeparator + '\n');
  buf.insert(prompt);
}

void CalcMode::insertChar(Buffer& buf, char c, Editor& ed) const {
  if (isPromptLine(buf)) {
    if (isOnPrompt(buf)) CMBAR_MSG(ed, "Cannot insert on prompt!");
    else buf.insert(c);
  } else {
    CMBAR_MSG(ed, "Cannot insert on a non-prompt line!");
  }
}

void CalcMode::evaluate(Buffer& buf, Editor& ed) {
  const auto& pt = buf.getPoint();
  const auto& line = buf.at(pt.y).get();
  if (line.compare(0, prompt.size(), prompt) != 0) {
    CMBAR_MSG(ed, "Cannot evaluate a non-expr line!");
    return;
  }
  auto expr = line.substr(prompt.size());
  if (expr.empty()) return;
  parser::StringScanner sc(expr);
  parser::Token tok;
  tok.type = parser::Token::End;
  do {
  } while(tok.type != parser::Token::End && tok.type != parser::Token::Unknown);
  //@todo: fix this
  buf.insert(format("\nResult: %s\n", expr.c_str()));
  printHeader(buf);  // insert the next prompt
}


std::vector<KeyCmdPair> CalcMode::Keys::All = {
  {" ", "calc-insert-char"},
  {"!", ""},
  {"\"", ""},
  {"#", ""},
  {"$", ""},
  {"%", ""},
  {"&", ""},
  {"'", ""},
  {"(", "calc-insert-char"},
  {")", "calc-insert-char"},
  {"*", "calc-insert-char"},
  {"+", "calc-insert-char"},
  {"=", "calc-insert-char"},
  {",", "calc-insert-char"},
  {"-", "calc-insert-char"},
  {".", "calc-insert-char"},
  {"/", "calc-insert-char"},
  {"0", "calc-insert-char"},
  {"1", "calc-insert-char"},
  {"2", "calc-insert-char"},
  {"3", "calc-insert-char"},
  {"4", "calc-insert-char"},
  {"5", "calc-insert-char"},
  {"6", "calc-insert-char"},
  {"7", "calc-insert-char"},
  {"8", "calc-insert-char"},
  {"9", "calc-insert-char"},
  {":", ""},
  {";", "calc-insert-char"},
  {"<", ""},
  {">", ""},
  {"?", ""},
  {"@", ""},
  {"A", "calc-insert-char"},
  {"B", "calc-insert-char"},
  {"C", "calc-insert-char"},
  {"D", "calc-insert-char"},
  {"E", "calc-insert-char"},
  {"F", "calc-insert-char"},
  {"G", "calc-insert-char"},
  {"H", "calc-insert-char"},
  {"I", "calc-insert-char"},
  {"J", "calc-insert-char"},
  {"K", "calc-insert-char"},
  {"L", "calc-insert-char"},
  {"M", "calc-insert-char"},
  {"N", "calc-insert-char"},
  {"O", "calc-insert-char"},
  {"P", "calc-insert-char"},
  {"Q", "calc-insert-char"},
  {"R", "calc-insert-char"},
  {"S", "calc-insert-char"},
  {"T", "calc-insert-char"},
  {"U", "calc-insert-char"},
  {"V", "calc-insert-char"},
  {"W", "calc-insert-char"},
  {"X", "calc-insert-char"},
  {"Y", "calc-insert-char"},
  {"Z", "calc-insert-char"},
  {"[", ""},
  {"\\", ""},
  {"]", ""},
  {"^", ""},
  {"_", "calc-insert-char"},
  {"`", ""},
  {"a", "calc-insert-char"},
  {"b", "calc-insert-char"},
  {"c", "calc-insert-char"},
  {"d", "calc-insert-char"},
  {"e", "calc-insert-char"},
  {"f", "calc-insert-char"},
  {"g", "calc-insert-char"},
  {"h", "calc-insert-char"},
  {"i", "calc-insert-char"},
  {"j", "calc-insert-char"},
  {"k", "calc-insert-char"},
  {"l", "calc-insert-char"},
  {"m", "calc-insert-char"},
  {"n", "calc-insert-char"},
  {"o", "calc-insert-char"},
  {"p", "calc-insert-char"},
  {"q", "calc-insert-char"},
  {"r", "calc-insert-char"},
  {"s", "calc-insert-char"},
  {"t", "calc-insert-char"},
  {"u", "calc-insert-char"},
  {"v", "calc-insert-char"},
  {"w", "calc-insert-char"},
  {"x", "calc-insert-char"},
  {"y", "calc-insert-char"},
  {"z", "calc-insert-char"},
  {"{", ""},
  {"|", "calc-insert-char"},
  {"}", ""},
  {"~", ""},
  {"enter", "calc-enter"},
  {"C-U", ""},
  {"C-R", ""},
  {"C-Y", ""},
  {"C-W", ""},
  {"tab", ""},
  {"backspace", "calc-backspace-char"},
  {"del", "calc-delete-char"},
  {"C-K", "calc-kill-line"},
};

std::vector<NameColorPair> CalcMode::Colors::All = {
};

REGISTER_MODE(CalcMode, "calc");

} // end namespace calc
} // end namespace teditor
