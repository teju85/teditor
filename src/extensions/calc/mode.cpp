#include "../base/text.h"
#include "core/buffer.h"

namespace teditor {
namespace calc {

/** calc mode */
class CalcMode: public text::TextMode {
public:
  CalcMode(): text::TextMode("calc") {
    populateKeyMap<CalcMode::Keys>(getKeyCmdMap());
    populateColorMap<CalcMode::Colors>(getColorMap());
  }

  static Mode* create() { return new CalcMode; }

  static bool modeCheck(const std::string& file) { return file == "*calc"; }

private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };
};

REGISTER_MODE(CalcMode, "calc");


std::vector<KeyCmdPair> CalcMode::Keys::All = {
  {" ", "insert-char"},
  {"!", "insert-char"},
  {"\"", "insert-char"},
  {"#", "insert-char"},
  {"$", "insert-char"},
  {"%", "insert-char"},
  {"&", "insert-char"},
  {"'", "insert-char"},
  {"(", "insert-char"},
  {")", "insert-char"},
  {"*", "insert-char"},
  {"+", "insert-char"},
  {"=", "insert-char"},
  {",", "insert-char"},
  {"-", "insert-char"},
  {".", "insert-char"},
  {"/", "insert-char"},
  {"0", "insert-char"},
  {"1", "insert-char"},
  {"2", "insert-char"},
  {"3", "insert-char"},
  {"4", "insert-char"},
  {"5", "insert-char"},
  {"6", "insert-char"},
  {"7", "insert-char"},
  {"8", "insert-char"},
  {"9", "insert-char"},
  {":", "insert-char"},
  {";", "insert-char"},
  {"<", "insert-char"},
  {">", "insert-char"},
  {"?", "insert-char"},
  {"@", "insert-char"},
  {"A", "insert-char"},
  {"B", "insert-char"},
  {"C", "insert-char"},
  {"D", "insert-char"},
  {"E", "insert-char"},
  {"F", "insert-char"},
  {"G", "insert-char"},
  {"H", "insert-char"},
  {"I", "insert-char"},
  {"J", "insert-char"},
  {"K", "insert-char"},
  {"L", "insert-char"},
  {"M", "insert-char"},
  {"N", "insert-char"},
  {"O", "insert-char"},
  {"P", "insert-char"},
  {"Q", "insert-char"},
  {"R", "insert-char"},
  {"S", "insert-char"},
  {"T", "insert-char"},
  {"U", "insert-char"},
  {"V", "insert-char"},
  {"W", "insert-char"},
  {"X", "insert-char"},
  {"Y", "insert-char"},
  {"Z", "insert-char"},
  {"[", "insert-char"},
  {"\\", "insert-char"},
  {"]", "insert-char"},
  {"^", "insert-char"},
  {"_", "insert-char"},
  {"`", "insert-char"},
  {"a", "insert-char"},
  {"b", "insert-char"},
  {"c", "insert-char"},
  {"d", "insert-char"},
  {"e", "insert-char"},
  {"f", "insert-char"},
  {"g", "insert-char"},
  {"h", "insert-char"},
  {"i", "insert-char"},
  {"j", "insert-char"},
  {"k", "insert-char"},
  {"l", "insert-char"},
  {"m", "insert-char"},
  {"n", "insert-char"},
  {"o", "insert-char"},
  {"p", "insert-char"},
  {"q", "insert-char"},
  {"r", "insert-char"},
  {"s", "insert-char"},
  {"t", "insert-char"},
  {"u", "insert-char"},
  {"v", "insert-char"},
  {"w", "insert-char"},
  {"x", "insert-char"},
  {"y", "insert-char"},
  {"z", "insert-char"},
  {"{", "insert-char"},
  {"|", "insert-char"},
  {"}", "insert-char"},
  {"~", "insert-char"},
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

} // end namespace calc
} // end namespace teditor
