#include "line.h"
#include <string.h>


namespace teditor {

Line::Line(): line() {
}

std::string Line::erase(int idx, int count) {
    std::string ret;
    if(idx >= length() || (idx+count) > length())
        return ret;
    ret = line.substr(idx, count);
    line.erase(line.begin()+idx, line.begin()+idx+count);
    return ret;
}

Line Line::split(int idx) {
    Line other;
    if(idx >= length() || idx < 0)
        return other;
    auto del = erase(idx, length()-idx);
    other.append(del);
    return other;
}

// This is wierd!! If empty check is not done, some garbage string seems to be
// appended everytime!
void Line::join(const Line& other) {
    if(!other.empty()) append(other.get());
}

void Line::prepend(char c, int count) {
    std::string tmp(count, c);
    prepend(tmp.c_str());
}

void Line::insert(char c, int idx) {
    if(idx >= length())
        append(c);
    else
        line.insert(line.begin()+idx, c);
}

void Line::insert(const char* c, int idx) {
    if(idx >= length())
        append(c);
    else
        line.insert(idx, c);
}

void Line::insert(const std::string& str, int idx) {
    if(idx >= length())
        append(str);
    else
        line.insert(idx, str);
}

int Line::numLinesNeeded(int wid) const {
    int len = length();
    // an empty line is still a line!
    if(len <= 0)
        return 1;
    return (len + wid - 1) / wid;
}

int Line::findFirstNotOf(const std::string& str, int pos) const {
    auto res = line.find_first_not_of(str, pos);
    if(res == std::string::npos)
        return line.length();
    return (int)res;
}

int Line::findLastNotOf(const std::string& str, int pos) const {
    auto res = line.find_last_not_of(str, pos);
    if(res == std::string::npos)
        return 0;
    return (int)res;
}

int Line::indentSize() const {
    auto pos = line.find_first_not_of(' ');
    if(pos == std::string::npos)
        return 0;
    return (int)pos;
}


bool LineCompare(const Line& a, const Line& b) {
    if(a.empty())
        return true;
    if(b.empty())
        return false;
    return strcmp(a.get().c_str(), b.get().c_str()) < 0;
}

} // end namespace teditor
