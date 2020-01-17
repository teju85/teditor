#include "regexs.h"

namespace teditor {
namespace parser {

const std::string
  Regexs::FloatingPt("[-+]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][-+]?[0-9]+)?");

const std::string Regexs::Integer("[-+]?[0-9]+");

const std::string Regexs::HexInt("0[xX][a-fA-F0-9]+");

const std::string Regexs::Variable("[a-zA-Z_][a-zA-Z0-9_]*");

const std::string Regexs::Newline("[\r\n]+");

}  // namespace parser
}  // namespace teditor
