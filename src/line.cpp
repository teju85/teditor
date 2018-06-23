#include "line.h"
#include <string.h>


namespace teditor {

std::string Line::erase(int idx, int count) {
    std::string ret;
    if(idx >= length() || (idx+count) > length())
        return ret;
    ret = line.substr(idx, count);
    line.erase(line.begin()+idx, line.begin()+idx+count);
    return ret;
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


bool LineCompare(const Line& a, const Line& b) {
    if(a.empty())
        return true;
    if(b.empty())
        return false;
    return strcmp(a.get().c_str(), b.get().c_str()) < 0;
}

} // end namespace teditor
