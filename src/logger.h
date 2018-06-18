#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string>

namespace teditor {

template <typename Clazz, typename ClazzArgs>
class SingletonHandler {
public:
    SingletonHandler(const ClazzArgs& args) {
        Clazz::inst = new Clazz(args);
    }

    ~SingletonHandler() {
        if(Clazz::inst != nullptr)
            delete Clazz::inst;
    }
};


class Logger {
public:
    static void setLevel(int le);
    static void log(int lev, const char* fmt, ...);
    static void messages(const char* fmt, ...);
    static void msgBar(const char* fmt, ...);
    static int logLevel();

private:
    Logger(const std::string& file_);
    ~Logger();
    void open();
    bool removeNewLine(char* buf, int& len);

    friend class SingletonHandler<Logger, std::string>;
    static Logger* inst;
    std::string file;
    FILE* logfp;
    int level;
};

#define CMBAR(fmt, ...) Logger::msgBar(fmt, ##__VA_ARGS__)
#define MESSAGE(fmt, ...) Logger::messages(fmt, ##__VA_ARGS__)

#define CMBAR_MSG(fmt, ...) do {                \
        CMBAR(fmt, ##__VA_ARGS__);             \
        MESSAGE(fmt, ##__VA_ARGS__);            \
    } while(0)

#define LOG(lev, fmt, ...) do {                                         \
        Logger::log(lev, "%s:%d: Lev=%d DLev=%d ", __FILE__, __LINE__,  \
                    lev, Logger::logLevel());                           \
        Logger::log(lev, fmt, ##__VA_ARGS__);                           \
    } while(0)

#define ULTRA_DEBUG(fmt, ...) LOG(100000, fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...) LOG(10000, fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) LOG(1000, fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) LOG(100, fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) LOG(10, fmt, ##__VA_ARGS__)
#define FATAL(fmt, ...) LOG(0, fmt, ##__VA_ARGS__)

} // end namespace teditor
