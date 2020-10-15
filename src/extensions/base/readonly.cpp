#include "readonly.h"
#include "core/utils.h"
#include "core/file_utils.h"


namespace teditor {
namespace readonly {

bool ReadOnlyMode::modeCheck(const std::string& file) {
  return isReadOnly(file.c_str());
}

ReadOnlyMode::ReadOnlyMode(const std::string& n, const std::string& w):
  Mode(n, w), kcMap(), cMap() {
  populateKeyMap<ReadOnlyMode::Keys>(getKeyCmdMap());
  getKeyCmdMap().resetTraversal();
  populateColorMap<ReadOnlyMode::Colors>(getColorMap());
}

void ReadOnlyMode::getColorFor(AttrColor& fg, AttrColor& bg, int lineNum,
                               int pos, const Buffer& b, bool isHighlighted) {
  fg = cMap.get(isHighlighted ? "highlightfg" : "defaultfg");
  bg = cMap.get(isHighlighted ? "highlightbg" : "defaultbg");
}

REGISTER_MODE(ReadOnlyMode, "ro");


std::vector<KeyCmdPair> ReadOnlyMode::Keys::All = {
  {"q", "kill-this-buffer"},
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
  {"M-right", "next-buffer"},
  {"M-left", "prev-buffer"},
  {"C-Q C-E", "quit"},
  {"C-Q C-O", "kill-other-buffers"},
  {"C-~", "start-region"},
  {"esc", "cancel"},
  {"F10", "find-file-history"},
  {"F11", "find-file"},
  {"F12", "scratch-buffer"},
  {"M-x", "run-command"},
  {"M-!", "shell-command"},
  {"F7", "browser-search"},
  {"F5", "reload-buffer"},
  {"M-w", "copy-region"},
  {"M-g", "goto-line"},
  {"C-B C-S", "buffer-switch"},
  {"C-F", "search"},
  {"C-G C-B", "git-branch"},
  {"F1", "help-command"},
  {"C-X 3", "split-vertically"},
  {"C-X 0", "clear-all-windows"},
  {"C-X o", "next-window"},
};

std::vector<NameColorPair> ReadOnlyMode::Colors::All = {
  {"clearfg",           "Black"},
  {"defaultfg",         "White"},
  {"defaultbg",         "DarkerGray"},
  {"highlightfg",       "defaultfg"},
  {"highlightbg",       "DarkestGray"},
  {"cursorfg",          "Black"},
  {"cursorbg",          "White"},
  {"winframebg",        "Black"},
  {"winframefg",        "White"},
  {"inactivecursorbg",  "DarkGray"},
  {"statusfg",          "DarkGreen"},
  {"statusbg",          "DarkestGray"},
  {"statusnamefg",      "Underline:Yellow"},
};

} // end namespace readonly
} // end namespace teditor
