#include "core/utils.h"
#include "number.h"
#include <unordered_map>
#include <string>

namespace teditor {
namespace calc {

typedef std::unordered_map<std::string, Num64> VarMap;

VarMap& vars();
History& cmds();

}  // namespace calc
}  // namespace teditor
