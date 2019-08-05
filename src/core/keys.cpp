#include "keys.h"
#include "utf8.h"
#include "logger.h"
#include "utils.h"


namespace teditor {

const key_t MetaKey::Mask = 0x0000FFFFU;

bool MetaKey::isAscii() const {
  key_t m = getMeta();
  if(m != Meta_None) return false;
  key_t k = getKey();
  return (Key_Space <= k && k <= Key_Tilde);
}

///@todo: handle mouse events
std::string MetaKey::toStr() const {
  std::string ret;
  if(isInvalid()) return ret;
  key_t m = getMeta();
  if(m & Meta_Ctrl)  ret += "C-";
  if(m & Meta_Alt)   ret += "M-";
  if(m & Meta_Shift) ret += "S-";
  key_t k = getKey();
  if(Key_F12 <= k && k <= Key_F1) {
    int num = 12 - (int)(k - Key_F12);
    ret += "F";
    ret += num2str(num);
  } else if(k == Key_Insert) ret += "insert";
  else if(k == Key_Delete) ret += "del";
  else if(k == Key_Home) ret += "home";
  else if(k == Key_End) ret += "end";
  else if(k == Key_PageUp) ret += "pageup";
  else if(k == Key_PageDown) ret += "pagedown";
  else if(k == Key_ArrowLeft) ret += "left";
  else if(k == Key_ArrowRight) ret += "right";
  else if(k == Key_ArrowDown) ret += "down";
  else if(k == Key_ArrowUp) ret += "up";
  else if(k == Key_CtrlTilde) ret += "~";
  else if(Key_CtrlA <= k && k <= Key_CtrlZ) {
    if(k == Key_Backspace) ret += "backspace";
    else if(k == Key_Tab) ret += "tab";
    else if(k == Key_Enter) ret += "enter";
    else {
      char num = (char)(k - Key_CtrlA) + 'A';
      ret += num;
    }
  } else if(k == Key_CtrlBackslash) ret += "\\";
  else if(k == Key_Ctrl6) ret += "6";
  else if(k == Key_CtrlUnderscore) ret += "_";
  else if(k == Key_Esc) ret += "esc";
  else if(Key_Space <= k && k <= Key_Tilde) ret += (char)k;
  else if(k == Key_Backspace2) ret += "backspace";
  return ret;
}

bool operator==(const MetaKey& mk1, const MetaKey& mk2) {
  return mk1.getFull() == mk2.getFull();
}

} // end namespace teditor
