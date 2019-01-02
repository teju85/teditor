#pragma once

#include <string>


namespace teditor {

///@todo: support for unicode in Line class

/** Base class to store a line in the buffer */
class Line {
public:
    /**
     * @defgroup Add Functions to append/prepend/insert chars to the line
     * @{
     */
    void append(char c) { line.push_back(c); }
    void append(const char* c) { line += c; }
    void append(const std::string& str) { line += str; }
    void prepend(char c) { insert(c, 0); }
    void prepend(const char* c) { insert(c, 0); }
    void prepend(char c, int count);
    void insert(char c, int idx);
    void insert(const char* c, int idx);
    void insert(const std::string& str, int idx) { insert(str.c_str(), idx); }
    /** @} */

    /** erases from the given index and returns the erased chars */
    std::string erase(int idx, int len=1);

    /**
     * @defgroup SplitJoin Functions to split and join the line back
     * @{
     */
    Line split(int idx);
    void join(const Line& other) { append(other.get()); }
    /** @} */

    /** number of lines needed on the screen to render this line */
    int numLinesNeeded(int wid) const;

    /** Check for empty line */
    bool empty() const { return line.empty(); }

    /** Number of chars in the line */
    int length() const { return (int)line.length(); }

    /** get the string */
    const std::string& get() const { return line; }

    /** access idx'th element in the line */
    char at(int idx) const { return line[idx]; }

    /** clear the line */
    void clear() { line.clear(); }

    /**
     * Same as find_first_not_of function of std::string
     * However, if no match, it will return the length of the line itself
     */
    int findFirstNotOf(const std::string& str, int pos) const;

    /**
     * Same as find_last_not_of function of std::string
     * However, if no match, it will return 0
     */
    int findLastNotOf(const std::string& str, int pos) const;

    /** measures the indentation at the beginning of this line */
    int indentSize() const;

private:
    /** string to be present on this line */
    std::string line;
};


/**
 * helper function to compare strings stored in the 2 lines.
 * Very useful as a functor to be passed for the 'sort' function
 */
bool LineCompare(const Line& a, const Line& b);

}; // end namespace teditor
