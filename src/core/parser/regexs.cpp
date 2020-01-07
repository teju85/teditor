#include "regexs.h"

namespace teditor {
namespace parser {

const std::string
  Regexs::FloatingPt("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?");

const std::string Regexs::Integer("[-+]?[0-9]+");

}  // namespace parser
}  // namespace teditor
