#pragma once

#include <string>


namespace teditor {

///@todo: support for unicode in Line class

/** Base class to store a line in the buffer */
class Line {
public:
  Line() {}

  /**
   * @defgroup Add Functions to append/prepend/insert chars to the line
   * @{
   */
  void append(char c) { line.push_back(c); }
  void append(const char* c) { line += c; }
  void append(const std::string& str) { line += str; }
  void prepend(char c) { insert(c, 0); }
  void prepend(const char* c) { insert(c, 0); }
  void prepend(char c, size_t count);
  void insert(char c, size_t idx);
  void insert(const char* c, size_t idx);
  void insert(const std::string& str, size_t idx);
  /** @} */

  /** erases from the given index and returns the erased chars */
  std::string erase(size_t idx, size_t len = 1);

  /**
   * @defgroup SplitJoin Functions to split and join the line back
   * @{
   */
  Line split(size_t idx);
  void join(const Line& other);
  /** @} */

  /** number of lines needed on the screen to render this line */
  size_t numLinesNeeded(size_t wid) const;

  /** Check for empty line */
  bool empty() const { return line.empty(); }

  /** Number of chars in the line */
  size_t length() const { return line.length(); }

  /** get the string */
  const std::string& get() const { return line; }

  /** access idx'th element in the line */
  char at(size_t idx) const { return line[idx]; }

  /** clear the line */
  void clear() { line.clear(); }

  /**
   * Same as find_first_not_of function of std::string
   * However, if no match, it will return the length of the line itself
   */
  size_t findFirstNotOf(const std::string& str, size_t pos) const;

  /**
   * Same as find_last_not_of function of std::string
   * However, if no match, it will return 0
   */
  size_t findLastNotOf(const std::string& str, size_t pos) const;

  /** measures the indentation at the beginning of this line */
  size_t indentSize() const;

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
