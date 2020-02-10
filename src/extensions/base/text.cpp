#include "text.h"
#include "core/buffer.h"
#include "core/parser/nfa.h"

namespace teditor {
namespace text {

REGISTER_MODE(TextMode, "text");

TextMode::TextMode(const std::string& n, const std::string& w):
  readonly::ReadOnlyMode(n, w) {
  populateKeyMap<TextMode::Keys>(getKeyCmdMap());
  populateColorMap<TextMode::Colors>(getColorMap());
}

int TextMode::indent(Buffer& buf, int line) {
  if(!(0 < line && line < buf.length())) return 0;
  const auto& prev = buf.at(line-1);
  auto& curr = buf.at(line);
  int prevInd = prev.indentSize();
  int currInd = curr.indentSize();
  return prevInd - currInd;
}

bool TextMode::modeCheck(const std::string& file) {
  static parser::NFA nfa(".*[.]txt");
  return nfa.find(file) != parser::NFA::NoMatch;
}

std::vector<KeyCmdPair> TextMode::Keys::All = {
  {" ", ".insert-char"},
  {"!", ".insert-char"},
  {"\"", ".insert-char"},
  {"#", ".insert-char"},
  {"$", ".insert-char"},
  {"%", ".insert-char"},
  {"&", ".insert-char"},
  {"'", ".insert-char"},
  {"(", ".insert-char"},
  {")", ".insert-char"},
  {"*", ".insert-char"},
  {"+", ".insert-char"},
  {"=", ".insert-char"},
  {",", ".insert-char"},
  {"-", ".insert-char"},
  {".", ".insert-char"},
  {"/", ".insert-char"},
  {"0", ".insert-char"},
  {"1", ".insert-char"},
  {"2", ".insert-char"},
  {"3", ".insert-char"},
  {"4", ".insert-char"},
  {"5", ".insert-char"},
  {"6", ".insert-char"},
  {"7", ".insert-char"},
  {"8", ".insert-char"},
  {"9", ".insert-char"},
  {":", ".insert-char"},
  {";", ".insert-char"},
  {"<", ".insert-char"},
  {">", ".insert-char"},
  {"?", ".insert-char"},
  {"@", ".insert-char"},
  {"A", ".insert-char"},
  {"B", ".insert-char"},
  {"C", ".insert-char"},
  {"D", ".insert-char"},
  {"E", ".insert-char"},
  {"F", ".insert-char"},
  {"G", ".insert-char"},
  {"H", ".insert-char"},
  {"I", ".insert-char"},
  {"J", ".insert-char"},
  {"K", ".insert-char"},
  {"L", ".insert-char"},
  {"M", ".insert-char"},
  {"N", ".insert-char"},
  {"O", ".insert-char"},
  {"P", ".insert-char"},
  {"Q", ".insert-char"},
  {"R", ".insert-char"},
  {"S", ".insert-char"},
  {"T", ".insert-char"},
  {"U", ".insert-char"},
  {"V", ".insert-char"},
  {"W", ".insert-char"},
  {"X", ".insert-char"},
  {"Y", ".insert-char"},
  {"Z", ".insert-char"},
  {"[", ".insert-char"},
  {"\\", ".insert-char"},
  {"]", ".insert-char"},
  {"^", ".insert-char"},
  {"_", ".insert-char"},
  {"`", ".insert-char"},
  {"a", ".insert-char"},
  {"b", ".insert-char"},
  {"c", ".insert-char"},
  {"d", ".insert-char"},
  {"e", ".insert-char"},
  {"f", ".insert-char"},
  {"g", ".insert-char"},
  {"h", ".insert-char"},
  {"i", ".insert-char"},
  {"j", ".insert-char"},
  {"k", ".insert-char"},
  {"l", ".insert-char"},
  {"m", ".insert-char"},
  {"n", ".insert-char"},
  {"o", ".insert-char"},
  {"p", ".insert-char"},
  {"q", ".insert-char"},
  {"r", ".insert-char"},
  {"s", ".insert-char"},
  {"t", ".insert-char"},
  {"u", ".insert-char"},
  {"v", ".insert-char"},
  {"w", ".insert-char"},
  {"x", ".insert-char"},
  {"y", ".insert-char"},
  {"z", ".insert-char"},
  {"{", ".insert-char"},
  {"|", ".insert-char"},
  {"}", ".insert-char"},
  {"~", ".insert-char"},
  {"enter", ".insert-char"},
  {"backspace", ".backspace-char"},
  {"del", ".delete-char"},
  {"C-S", "save-buffer"},
  {"C-Q C-T", "kill-this-buffer"},
  {"C-K", "kill-line"},
  {"C-U", "command-undo"},
  {"C-R", "command-redo"},
  {"C-Y", "paste-region"},
  {"C-W", "cut-region"},
  {"tab", "indent"}
};

std::vector<NameColorPair> TextMode::Colors::All;

} // end namespace text
} // end namespace teditor
