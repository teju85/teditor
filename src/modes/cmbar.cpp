#include "core/mode.h"
#include "core/buffer.h"
#include "core/key_cmd_map.h"
#include "core/colors.h"


namespace teditor {


/** cmbar mode */
class CmBarMode: public Mode {
public:
    CmBarMode();
    int indent(Buffer& buf, int line) { return 0; }
    KeyCmdMap& getKeyCmdMap() { return kcMap; }
    ColorMap& getColorMap() { return cMap; }

    static Mode* create() { return new CmBarMode; }

    static bool modeCheck(const std::string& file) { return false; }

private:
    KeyCmdMap kcMap;
    ColorMap cMap;

    struct Keys { static std::vector<KeyCmdPair> All; };
    struct Colors { static std::vector<NameColorPair> All; };
};

REGISTER_MODE(CmBarMode, "cmbar");


CmBarMode::CmBarMode():
    Mode("cmbar",
         "abcdefghijklmnopqrstuvwxyzABCDEGGHIJKLMNOPQRSTUVWXYZ0123456789_-"),
    kcMap(), cMap() {
    populateKeyMap<CmBarMode::Keys>(kcMap);
    kcMap.resetTraversal();
    populateColorMap<CmBarMode::Colors>(cMap);
}


std::vector<KeyCmdPair> CmBarMode::Keys::All = {
    {" ", "prompt-insert-char"},
    {"!", "prompt-insert-char"},
    {"\"", "prompt-insert-char"},
    {"#", "prompt-insert-char"},
    {"$", "prompt-insert-char"},
    {"%", "prompt-insert-char"},
    {"&", "prompt-insert-char"},
    {"'", "prompt-insert-char"},
    {"(", "prompt-insert-char"},
    {")", "prompt-insert-char"},
    {"*", "prompt-insert-char"},
    {"+", "prompt-insert-char"},
    {",", "prompt-insert-char"},
    {"-", "prompt-insert-char"},
    {".", "prompt-insert-char"},
    {"/", "prompt-insert-char"},
    {"0", "prompt-insert-char"},
    {"1", "prompt-insert-char"},
    {"2", "prompt-insert-char"},
    {"3", "prompt-insert-char"},
    {"4", "prompt-insert-char"},
    {"5", "prompt-insert-char"},
    {"6", "prompt-insert-char"},
    {"7", "prompt-insert-char"},
    {"8", "prompt-insert-char"},
    {"9", "prompt-insert-char"},
    {":", "prompt-insert-char"},
    {";", "prompt-insert-char"},
    {"<", "prompt-insert-char"},
    {"?", "prompt-insert-char"},
    {"@", "prompt-insert-char"},
    {"A", "prompt-insert-char"},
    {"B", "prompt-insert-char"},
    {"C", "prompt-insert-char"},
    {"D", "prompt-insert-char"},
    {"E", "prompt-insert-char"},
    {"F", "prompt-insert-char"},
    {"G", "prompt-insert-char"},
    {"H", "prompt-insert-char"},
    {"I", "prompt-insert-char"},
    {"J", "prompt-insert-char"},
    {"K", "prompt-insert-char"},
    {"L", "prompt-insert-char"},
    {"M", "prompt-insert-char"},
    {"N", "prompt-insert-char"},
    {"O", "prompt-insert-char"},
    {"P", "prompt-insert-char"},
    {"Q", "prompt-insert-char"},
    {"R", "prompt-insert-char"},
    {"S", "prompt-insert-char"},
    {"T", "prompt-insert-char"},
    {"U", "prompt-insert-char"},
    {"V", "prompt-insert-char"},
    {"W", "prompt-insert-char"},
    {"X", "prompt-insert-char"},
    {"Y", "prompt-insert-char"},
    {"Z", "prompt-insert-char"},
    {"[", "prompt-insert-char"},
    {"\\", "prompt-insert-char"},
    {"]", "prompt-insert-char"},
    {"^", "prompt-insert-char"},
    {"_", "prompt-insert-char"},
    {"`", "prompt-insert-char"},
    {"a", "prompt-insert-char"},
    {"b", "prompt-insert-char"},
    {"c", "prompt-insert-char"},
    {"d", "prompt-insert-char"},
    {"e", "prompt-insert-char"},
    {"f", "prompt-insert-char"},
    {"g", "prompt-insert-char"},
    {"h", "prompt-insert-char"},
    {"i", "prompt-insert-char"},
    {"j", "prompt-insert-char"},
    {"k", "prompt-insert-char"},
    {"l", "prompt-insert-char"},
    {"m", "prompt-insert-char"},
    {"n", "prompt-insert-char"},
    {"o", "prompt-insert-char"},
    {"p", "prompt-insert-char"},
    {"q", "prompt-insert-char"},
    {"r", "prompt-insert-char"},
    {"s", "prompt-insert-char"},
    {"t", "prompt-insert-char"},
    {"u", "prompt-insert-char"},
    {"v", "prompt-insert-char"},
    {"w", "prompt-insert-char"},
    {"x", "prompt-insert-char"},
    {"y", "prompt-insert-char"},
    {"z", "prompt-insert-char"},
    {"{", "prompt-insert-char"},
    {"|", "prompt-insert-char"},
    {"}", "prompt-insert-char"},
    {"~", "prompt-insert-char"},
    {"enter", "prompt-quit"},
    {"esc", "prompt-cancel"},
    {"right", "prompt-cursor-right"},
    {"left", "prompt-cursor-left"},
    {"home", "prompt-cursor-home"},
    {"end", "prompt-cursor-line-end"},
    {"backspace", "prompt-backspace-char"},
    {"del", "prompt-delete-char"},
    {"down", "prompt-options-down"},
    {"up", "prompt-options-up"}
};

std::vector<NameColorPair> CmBarMode::Colors::All = {
    {"defaultfg",         "White"},
    {"defaultbg",         "DarkerGray"},
    {"highlightfg",       "defaultfg"},
    {"highlightbg",       "DarkestGray"},
    {"cursorfg",          "Black"},
    {"cursorbg",          "White"},
    {"inactivecursorbg",  "DarkGray"},
    {"statusfg",          "DarkGreen"},
    {"statusbg",          "DarkestGray"},
    {"statusnamefg",      "Underline:Yellow"},
    {"cmbarfg",           "defaultfg"},
    {"cmbarbg",           "defaultbg"},
    {"cmbarhighlightfg",  "highlightfg"},
    {"cmbarhighlightbg",  "highlightbg"}
};


} // end namespace teditor
