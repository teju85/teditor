#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include "utils.h"

namespace teditor {

template <typename Clazz, typename ClazzArgs>
class SingletonHandler {
public:
  SingletonHandler(const ClazzArgs& args) {
    ASSERT(Clazz::inst == nullptr,
           "Singleton object is already initialized!");
    Clazz::inst = new Clazz(args);
  }

  ~SingletonHandler() {
    if(Clazz::inst != nullptr) delete Clazz::inst;
    Clazz::inst = nullptr;
  }
};


class Editor;

class Logger {
public:
  static void setLevel(int le);
  static void log(int lev, const char* fmt, ...);
  static void messages(Editor& ed, const char* fmt, ...);
  static void msgBar(Editor& ed, const char* fmt, ...);
  static int logLevel();

private:
  Logger(const std::string& file_);
  ~Logger();
  void open();
  bool removeNewLine(std::string& buf);

  friend class SingletonHandler<Logger, std::string>;
  static Logger* inst;
  std::string file;
  FILE* logfp;
  int level;
};

#define CMBAR(ed, fmt, ...) Logger::msgBar(ed, fmt, ##__VA_ARGS__)
#define MESSAGE(ed, fmt, ...) Logger::messages(ed, fmt, ##__VA_ARGS__)

#define CMBAR_MSG(ed, fmt, ...) do {            \
    CMBAR(ed, fmt, ##__VA_ARGS__);              \
    MESSAGE(ed, fmt, ##__VA_ARGS__);            \
  } while(0)

#define LOG(lev, fmt, ...) do {                                         \
    Logger::log(lev, "%s:%d: Lev=%d DLev=%d ", __FILE__, __LINE__,      \
                lev, Logger::logLevel());                               \
    Logger::log(lev, fmt, ##__VA_ARGS__);                               \
  } while(0)

#define FATAL(fmt, ...) LOG(0, fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) LOG(10, fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) LOG(100, fmt, ##__VA_ARGS__)

#ifdef DEBUG_BUILD
#define INFO(fmt, ...) LOG(1000, fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...) LOG(10000, fmt, ##__VA_ARGS__)
#define ULTRA_DEBUG(fmt, ...) LOG(100000, fmt, ##__VA_ARGS__)
#else   // DEBUG_BUILD
#define INFO(fmt, ...)
#define DEBUG(fmt, ...)
#define ULTRA_DEBUG(fmt, ...)
#endif  // DEBUG_BUILD

} // end namespace teditor
