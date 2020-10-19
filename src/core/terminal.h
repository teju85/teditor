#pragma once

#include <string>
#include "utils.h"
#include <string.h>
#include <termios.h>
#include "pos2d.h"
#include "keys.h"
#include "logger.h"

namespace teditor {

// [0m sgr0   Reset all attributes
// [1m bold   Set "bright" attribute
// [2m dim    Set "dim" attribute
// [3m smso   Set "standout" attribute
// [4m set    Set "underscore" (underlined text) attribute
// [5m blink  Set "blink" attribute
// [8m invis  Set "hidden" attribute
enum Func {
  Func_EnterCA,
  Func_ExitCA,
  Func_ShowCursor,
  Func_HideCursor,
  Func_ClearScreen,
  Func_Sgr0,
  Func_Underline,
  Func_Bold,
  Func_Blink,
  Func_EnterKeypad,
  Func_ExitKeypad,
  Func_EnterMouse,
  Func_ExitMouse,
  Func_FuncsNum,
};

enum EventType {
  Event_Key = 1,
  Event_Resize,
  Event_Mouse,
  Event_None = 0
};

const char* func2terminfo(Func f);


///@todo: implement decodeUtf8 if needed
///@todo: add support for mouse in future
class Terminal {
public:
  /** type of the event */
  EventType type;
  /** key info */
  MetaKey mk;
  /** for mouse events */
  Pos2d<uint16_t> loc;

  int getWinchFd(int idx) const { return winchFds[idx]; }
  int width() const { return tsize.x; }
  int height() const { return tsize.y; }

  /**
   * @defgroup PtyOps Operations to interact with the underlying pty
   * @{
   */
  /**
   * @brief append data at the end of the current buffer contents
   * @param data the data to be appended
   * @param len number of elements in the data
   */
  void puts(const char* data, size_t len);
  void puts(const char* data) { puts(data, strlen(data)); }
  void puts(const std::string& data) { puts(data.c_str(), data.length()); }
  void puts(Func f) { puts(func(f)); }
  /** flush the contents of the buffer to the pty */
  void flush();
  /** update the terminal size */
  void updateTermSize();
  /** @} */

  /**
   * @defgroup InputOps Operations for reading events from the pty
   * @{
   */
  /** reset the input pipeline */
  void reset();
  /** wait for an event and extract it */
  int waitAndFill(struct timeval* timeout);
  /** get the previous key sequence */
  const std::string& getOldSeq() const { return oldSeq; }
  /** whether to resize the buffer */
  bool bufferResize() const { return buffResize; }
  /** disable the resize of buffer */
  void disableResize() { buffResize = false; }
  /** @} */

  /** get the singleton instance */
  static Terminal& getInstance();

  /** flag to raise an undefined escape sequence scenario */
  static const int UndefinedSequence;

private:
  /** functions */
  Strings funcs;
  /** pty name */
  std::string termName;
  /** buffer used to communicate commands to the pty */
  std::string outbuff;
  /** tty file to be used for communication */
  std::string ttyFile;
  /** file descriptor for the ttyFile */
  int inout;
  /** termios struct to update attrs */
  struct termios tios, origTios;
  /** terminal size */
  Pos2di tsize;
  /** key sequences */
  std::string seq, oldSeq;
  /** whether to resize the buffer */
  bool buffResize;
  /** window change listeners */
  int winchFds[2];

  /** the singleton object */
  static Terminal* inst;
  static const std::string EnterMouseSeq;
  static const std::string ExitMouseSeq;
  static const int BuffSize;

  enum ColorSupport {
    CS_None = 0,
    CS_256,
    CS_True
  };

  Terminal(const std::string& tty);
  ~Terminal();
  void setSignalHandler();
  void setupTios();
  ColorSupport colorSupported() const;
  const char* func(int id) const { return funcs[id].c_str(); }
  std::string tryReading(const char* path, const char* term) const;
  std::string loadTerminfo() const;
  std::string copyString(const std::string& tidata, int str, int table) const;
  int readAndExtract();
  int decodeChar(key_t ch);
  int decodeEscSeq();

  template <typename A, typename B> friend class SingletonHandler;
};

}; // end namespace teditor
