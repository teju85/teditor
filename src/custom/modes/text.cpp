#include "text.h"
#include "buffer.h"
#include "pcre.h"


namespace teditor {

REGISTER_MODE(TextMode, "text");


TextMode::TextMode():
    Mode("text",
         "abcdefghijklmnopqrstuvwxyzABCDEGGHIJKLMNOPQRSTUVWXYZ0123456789_"),
    kcMap(), cMap() {
    populateKeyMap<TextMode::Keys>(kcMap);
    kcMap.resetTraversal();
    populateColorMap<TextMode::Colors>(cMap);
}

int TextMode::indent(Buffer& buf, int line) {
    if(!(0 < line && line < buf.length()))
        return 0;
    const auto& prev = buf.at(line-1);
    auto& curr = buf.at(line);
    int prevInd = prev.indentSize();
    int currInd = curr.indentSize();
    return std::max(0, prevInd-currInd);
}

bool TextMode::modeCheck(const std::string& file) {
    static Pcre reg("[.]txt$");
    return reg.isMatch(file);
}


std::vector<KeyCmdPair> TextMode::Keys::All = {
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
    {"enter", "insert-char"},
    {"down", "cursor-down"},
    {"up", "cursor-up"},
    {"right", "cursor-right"},
    {"left", "cursor-left"},
    {"C-home", "cursor-reset"},
    {"C-end", "cursor-end"},
    {"home", "cursor-home"},
    {"end", "cursor-line-end"},
    {"pagedown", "page-down"},
    {"pageup", "page-up"},
    {"C-up", "previous-para"},
    {"C-down", "next-para"},
    {"C-left", "previous-word"},
    {"C-P", "match-paren"},
    {"C-right", "next-word"},
    {"backspace", "backspace-char"},
    {"del", "delete-char"},
    {"C-S", "buffer-save"},
    {"M-right", "buffer-next"},
    {"M-left", "buffer-prev"},
    {"C-Q C-E", "quit"},
    {"C-Q C-T", "kill-this-buffer"},
    {"C-Q C-O", "kill-other-buffers"},
    {"C-~", "start-region"},
    {"esc", "cancel"},
    {"F10", "find-file-history"},
    {"F11", "find-file"},
    {"F12", "scratch-buffer"},
    {"M-x", "run-command"},
    {"M-!", "shell-command"},
    {"F7", "browser-search"},
    {"C-K", "kill-line"},
    {"C-Z", "command-undo"},
    {"C-Y", "command-redo"},
    {"F5", "reload-buffer"},
    {"C-D", "add-cursor-down"},
    {"C-U", "add-cursor-up"},
    {"C-C", "copy-region"},
    {"C-V", "paste-region"},
    {"C-X", "cut-region"},
    {"M-g", "goto-line"},
    {"C-B C-S", "buffer-switch"},
    {"C-F", "search"},
    {"C-G C-B", "git-branch"},
    {"tab", "indent"}
};

std::vector<NameColorPair> TextMode::Colors::All = {
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
