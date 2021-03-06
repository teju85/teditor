#include "terminal.h"
#include <cstdio>
#include <sys/stat.h>
#include <stdint.h>
#include "logger.h"
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <vector>
#include <unordered_map>
#include <signal.h>
#include "file_utils.h"
#include "infocmp.h"


namespace teditor {

// ref: https://invisible-island.net/ncurses/man/terminfo.5.html
const char* func2terminfo(Func f) {
  switch (f) {
  case Func_EnterCA: return "smcup";
  case Func_ExitCA: return "rmcup";
  case Func_ShowCursor: return "cnorm";
  case Func_HideCursor: return "civis";
  case Func_ClearScreen: return "clear";
  case Func_Sgr0: return "sgr0";
  case Func_Underline: return "smul";
  case Func_Italic: return "sitm";
  case Func_Bold: return "bold";
  case Func_Blink: return "blink";
  case Func_EnterKeypad: return "smkx";
  case Func_ExitKeypad: return "rmkx";
  case Func_EnterMouse: return "TBD";
  case Func_ExitMouse: return "TBD";
  default:
    ASSERT(false, "func2terminfo: bad func passed '%d'!", f);
  };
}

void exitGracefully(int signum) { exit(signum); }

void sigwinch_handler(int xxx) {
  (void) xxx;
  const int zzz = 1;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
  (void)write(Terminal::getInstance().getWinchFd(1), &zzz, sizeof(int));
#pragma GCC diagnostic pop
}

const std::string Terminal::EnterMouseSeq = "\x1b[?1000h\x1b[?1002h\x1b[?1015h\x1b[?1006h";
const std::string Terminal::ExitMouseSeq = "\x1b[?1006l\x1b[?1015l\x1b[?1002l\x1b[?1000l";
const int Terminal::BuffSize = 32 * 1024;
const int Terminal::UndefinedSequence = -10;
Terminal* Terminal::inst = nullptr;

Terminal& Terminal::getInstance() {
  ASSERT(inst != nullptr, "Terminal has not been initialized!");
  return *inst;
}

void Terminal::puts(const char* data, size_t len) {
  auto currCap = outbuff.capacity();
  auto requiredCap = outbuff.length() + len;
  while(currCap < requiredCap) currCap *= 2;
  if(outbuff.capacity() != currCap) outbuff.reserve(currCap);
  ULTRA_DEBUG("puts: data='%s' len=%u\n", data, len);
  outbuff.append(data, len);
}

void Terminal::flush() {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
  (void)write(inout, outbuff.c_str(), outbuff.length());
#pragma GCC diagnostic pop
  ULTRA_DEBUG("flush: len=%lu buf=%s\n", outbuff.length(), outbuff.c_str());
  outbuff.clear();
}

void Terminal::updateTermSize() {
  struct winsize sz;
  memset(&sz, 0, sizeof(sz));
  ioctl(inout, TIOCGWINSZ, &sz);
  tsize.x = sz.ws_col;
  tsize.y = sz.ws_row;
}

Terminal::Terminal(const std::string& tty):
  type(), mk(), loc(), funcs(), termName(env("TERM")), outbuff(), ttyFile(tty),
  inout(-1), tios(), origTios(), seq(), oldSeq(), buffResize(false),
  winchFds() {
  // terminfo setup
  InfoCmp infocmp;
  for (int i = 0; i < Func_FuncsNum - 2; ++i) {
    funcs.push_back(infocmp.getStrCap(func2terminfo(Func(i))));
  }
  funcs.push_back(EnterMouseSeq);
  funcs.push_back(ExitMouseSeq);
  INFO("Terminal: term=%s ttyFile=%s\n", termName.c_str(), ttyFile.c_str());
  // termios setup
  inout = open(ttyFile.c_str(), O_RDWR);
  ASSERT(inout >= 0, "Terminal: Failed to open tty '%s'!", ttyFile.c_str());
  auto f = fcntl(inout, F_SETFL, O_NONBLOCK);
  ASSERT(f >= 0, "Terminal: failed to fcntl in the tty descriptor (ret=%d)!", f);
  ASSERT(isatty(inout), "Terminal: descriptor opened from '%s' not a tty!",
         ttyFile.c_str());
  outbuff.reserve(BuffSize);
  setupTios();
  puts(Func_EnterKeypad);
  puts(Func_EnterCA);
  puts(Func_HideCursor); // we'll manually handle the cursor draws
  // event handler setup
  ASSERT(pipe(winchFds) >= 0, "Failed to setup 'pipe'!");
  reset();
  setSignalHandler();
}

Terminal::~Terminal() {
  puts(Func_Sgr0);
  puts(Func_ExitCA);
  puts(Func_ExitKeypad);
  puts(Func_ExitMouse);
  puts(Func_ShowCursor);
  flush();
  tcsetattr(inout, TCSAFLUSH, &origTios);
  close(inout);
  close(winchFds[0]);
  close(winchFds[1]);
}

void Terminal::setSignalHandler() {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sigwinch_handler;
  sa.sa_flags = 0;
  sigaction(SIGWINCH, &sa, 0);
  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = exitGracefully;
  sigaction(SIGTERM, &action, NULL);
  sigaction(SIGINT, &action, NULL);
  sigaction(SIGQUIT, &action, NULL);
  sigaction(SIGHUP, &action, NULL);
  signal(SIGPIPE, SIG_IGN);
}

void Terminal::setupTios() {
  tcgetattr(inout, &origTios);
  memcpy(&tios, &origTios, sizeof(tios));
  tios.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                    | INLCR | IGNCR | ICRNL | IXON); // INPCK
  tios.c_oflag &= ~OPOST;
  tios.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  tios.c_cflag &= ~(CSIZE | PARENB);
  tios.c_cflag |= CS8;
  tios.c_cc[VMIN] = 0;  // Return each byte, or zero for timeout.
  tios.c_cc[VTIME] = 0; // 0ms timeout (unit is tens of second).
  tcsetattr(inout, TCSAFLUSH, &tios);
}

Terminal::ColorSupport Terminal::colorSupported() const {
  std::string colorterm = env("COLORTERM");
  if((colorterm.find("truecolor") != std::string::npos) ||
     (colorterm.find("24bit") != std::string::npos)) {
    return CS_True;
  }
  if((termName.find("-256") != std::string::npos) ||
     termName == "xterm") {
    return CS_256;
  }
  return CS_None;
}


struct KeyCombo {
  MetaKey mk;
  std::string escSeq;
  static const std::vector<KeyCombo> Combos;
};

#include "xterm_strokes.h"

struct AllCombos {
  AllCombos() { for(auto& c : KeyCombo::Combos) allKeys[c.escSeq] = c.mk; }
  static std::unordered_map<std::string, MetaKey> allKeys;
  static const AllCombos test;
};

std::unordered_map<std::string, MetaKey> AllCombos::allKeys;
const AllCombos AllCombos::test;

void Terminal::reset() {
  type = Event_None;
  loc = {0, 0};
  mk.reset();
  mk.setMeta(Meta_None);
}

int Terminal::decodeChar(key_t ch) {
  ULTRA_DEBUG("decodeChar: ch=%u\n", ch);
  mk.setKey(ch);
  // Ctrl A-Z and 0-7, except Enter
  if(ch < Key_Space && ch != Key_Enter && ch != Key_Tab)
    mk.updateMeta(Meta_Ctrl);
  return 1;
}

int Terminal::decodeEscSeq() {
  int len = (int)seq.length();
  ULTRA_DEBUG("decodeEscSeq: len=%d seq=%s\n", len, seq.c_str());
  if(len == 0) {
    mk.setKey(Key_Esc);
    return 1;
  }
  // Alt-<key>
  if(len == 1 && (char)Key_Space <= seq[0] && seq[0] <= (char)Key_Tilde) {
    mk = MetaKey(Meta_Alt, (key_t)seq[0]);
    seq.erase(0, 1);
    return 1;
  }
  // check for shortest string matching the list of keys
  for(int i=1;i<=len;++i) {
    std::string str(seq, 0, i);
    const auto itr = AllCombos::allKeys.find(str);
    if(itr != AllCombos::allKeys.end()) {
      mk = itr->second;
      seq.erase(0, i);
      return i;
    }
  }
  oldSeq = seq;
  seq.clear();
  DEBUG("decodeEscSeq: unknown seq=%s\n", oldSeq.c_str());
  return UndefinedSequence;
}

int Terminal::readAndExtract() {
  ULTRA_DEBUG("Terminal::readAndExtract: before read seq=%s\n", seq.c_str());
  if (seq.empty()) {
    int rs = 1;
    do {
      char c;
      rs = read(inout, &c, 1);
      if(rs > 0)
        seq += c;
    } while(rs > 0);
  }
  ULTRA_DEBUG("Terminal::readAndExtract: after read seq=%s\n", seq.c_str());
  ///@todo: support for mouse events
  type = Event_Key;
  mk.setMeta(Meta_None);
  char c = seq[0];
  seq.erase(0, 1);
  if(c == (char)Key_Esc) return decodeEscSeq();
  if(c <= (char)Key_Backspace2) return decodeChar(c);
  ASSERT(false, "Terminal::readAndExtract: UTF8 sequence found '%u'!",
         (uint32_t)c);
}

int Terminal::readKey() {
  auto n = readAndExtract();
  ULTRA_DEBUG("Terminal::readKey: inout keyval=%d\n", n);
  if(n == UndefinedSequence) return n;
  if(n < 0) return -1;
  if(n > 0) return type;
  return 0;
}

int Terminal::waitAndFill(struct timeval* timeout) {
  fd_set events;
  reset();
  while(1) {
    FD_ZERO(&events);
    FD_SET(inout, &events);
    FD_SET(winchFds[0], &events);
    int maxfd  = std::max(winchFds[0], inout);
    if (!seq.empty()) return readKey();
    ULTRA_DEBUG("Terminal::waitAndFill: waiting on select...\n");
    int result = select(maxfd+1, &events, 0, 0, timeout);
    ULTRA_DEBUG("Terminal::waitAndFill: result=%d\n", result);
    if(!result) return 0;
    // resize event
    if(FD_ISSET(winchFds[0], &events)) {
      type = Event_Resize;
      int zzz = 0;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
      read(winchFds[0], &zzz, sizeof(int));
#pragma GCC diagnostic pop
      buffResize = true;
      ULTRA_DEBUG("Terminal::waitAndFill: winchFds event type=%d\n", type);
      return type;
    } else {
      ULTRA_DEBUG("Terminal::waitAndFill: winchFds disabling resize\n");
      disableResize();
    }
    // key/mouse events
    if(FD_ISSET(inout, &events)) return readKey();
  }
}

} // end namespace teditor
