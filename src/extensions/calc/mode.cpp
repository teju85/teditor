#include "core/buffer.h"
#include "core/option.h"
#include "mode.h"

namespace teditor {
namespace calc {

CalcMode::CalcMode():
  text::TextMode("calc"), vars(), prompt(Option::get("calc:prompt").getStr()),
  lineSeparator(Option::get("calc:lineSeparator").getStr()) {
  populateKeyMap<CalcMode::Keys>(getKeyCmdMap());
  populateColorMap<CalcMode::Colors>(getColorMap());
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
  {"enter", ""},   ///@todo!
  {"C-K", ""},
  {"C-U", ""},
  {"C-R", ""},
  {"C-Y", ""},
  {"C-W", ""},
  {"tab", ""}
};

std::vector<NameColorPair> CalcMode::Colors::All = {
};

REGISTER_MODE(CalcMode, "calc");

} // end namespace calc
} // end namespace teditor
