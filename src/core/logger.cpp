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
    fprintf(inst->logfp, buf.c_str());
    fflush(inst->logfp);
}

void Logger::msgBar(const char* fmt, ...) {
    if(!Editor::instanceReady())
        return;
    va_list vl;
    va_start(vl, fmt);
    auto buf = format(fmt, vl);
    va_end(vl);
    int len;
    inst->removeNewLine((char*)buf.c_str(), len);
    auto& ed = Editor::getInstance();
    auto& cmBar = ed.getCmBar();
    cmBar.clear();
    cmBar.insert(buf.c_str());
}

void Logger::messages(const char* fmt, ...) {
    if(!Editor::instanceReady())
        return;
    va_list vl;
    va_start(vl, fmt);
    auto buf = format(fmt, vl);
    va_end(vl);
    auto& mlb = Editor::getInstance().getMessagesBuff();
    mlb.insert(buf.c_str());
}

} // end namespace teditor
