#pragma once

#include <string>


namespace teditor {

///@todo: support for unicode
class Line {
public:
    void append(char c) { line.push_back(c); }
    void append(const char* c) { line += c; }
    void append(const std::string& str) { line += str; }
    void prepend(char c) { insert(c, 0); }
    void prepend(const char* c) { insert(c, 0); }
    void insert(char c, int idx);
    void insert(const char* c, int idx);
    void insert(const std::string& str, int idx) { insert(str.c_str(), idx); }
    std::string erase(int idx, int len=1);
    int numLinesNeeded(int wid) const;
    bool empty() const { return length() == 0; }
    int length() const { return (int)line.length(); }
    const std::string& get() const { return line; }
    char at(int idx) const { return line[idx]; }
    void clear() { line.clear(); }
    int findFirstNotOf(const std::string& str, int pos) const;
    int findLastNotOf(const std::string& str, int pos) const;

private:
    std::string line;
};

}; // end namespace teditor
