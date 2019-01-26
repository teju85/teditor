#include "logger.h"
#include "utils.h"
#include "editor.h"
#include <string.h>


namespace teditor {

Logger* Logger::inst = nullptr;

void Logger::setLevel(int le) {
    if(inst != nullptr)
        inst->level = le;
}

int Logger::logLevel() {
    return (inst != nullptr)? inst->level : -1;
}

Logger::Logger(const std::string& file_): file(file_), logfp(nullptr),
                                          level(-1) {
}

Logger::~Logger() {
    if(logfp != nullptr)
        fclose(logfp);
}

void Logger::open() {
    if(logfp == nullptr) {
        logfp = fopen(file.c_str(), "w");
        ASSERT(logfp, "Failed to open file '%s'!", file.c_str());
    }
}

bool Logger::removeNewLine(char* buf, int& len) {
    len = strlen(buf);
    bool newLineFound = false;
    if(buf[len-1] == '\n') {
        buf[len-1] = '\0';
        newLineFound = true;
        --len;
        if(len == 0)
            newLineFound = false;
    }
    return newLineFound;
}

void Logger::log(int lev, const char* fmt, ...) {
    if(inst == nullptr || lev > inst->level)
        return;
    inst->open();
    va_list vl;
    va_start(vl, fmt);
    auto buf = format(fmt, vl);
    va_end(vl);
    fprintf(inst->logfp, "%s", buf.c_str());
    fflush(inst->logfp);
}

void Logger::msgBar(Editor& ed, const char* fmt, ...) {
    va_list vl;
    va_start(vl, fmt);
    auto str = format(fmt, vl);
    va_end(vl);
    int len;
    inst->removeNewLine((char*)str.c_str(), len);
    auto& cmBar = ed.getCmBar();
    cmBar.clear();
    cmBar.insert(str);
}

void Logger::messages(Editor& ed, const char* fmt, ...) {
    va_list vl;
    va_start(vl, fmt);
    auto str = format(fmt, vl);
    va_end(vl);
    auto& buf = ed.getMessagesBuff();
    buf.insert(str);
}

} // end namespace teditor
