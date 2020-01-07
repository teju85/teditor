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
};  // struct Regexs

}  // namespace parser
}  // namespace teditor
