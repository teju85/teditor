#pragma once

#include <string>

namespace teditor {
namespace parser {

/**
 * @brief A collection of commonly used Regular expressions
 */
struct Regexs {
  static const std::string FloatingPt;   ///< floating point number
  static const std::string Integer;      ///< integer
  static const std::string HexInt;       ///< hexadecimal integer
  static const std::string Variable;     ///< variable names in a program
  static const std::string Newline;      ///< newline (windows as well as unix)
  static const std::string DQuotedStr;   ///< double-quoted string
  static const std::string Whitespace;   ///< all whitespace chars
};  // struct Regexs

}  // namespace parser
}  // namespace teditor
