#include "command.h"
#include "editor.h"
#include "utils.h"


namespace teditor {

CommandMap& cmds() {
  static CommandMap _cmds;
  return _cmds;
}

CmdRegistrar::CmdRegistrar(const std::string& cmd, OperateFunc op,
                           const std::string& grp) {
  auto& c = cmds();
  ASSERT(c.find(cmd) == c.end(),
         "Command '%s' already registered!", cmd.c_str());
  c[cmd] = std::make_pair(op, grp);
}

bool defaultCmdFilterOp(const std::string& name) { return true; }

const Command& getCmd(const std::string& cmd) {
  auto& c = cmds();
  const auto itr = c.find(cmd);
  ASSERT(itr != c.end(), "Command '%s' not registered!", cmd.c_str());
  return itr->second;
}

Strings allCmdNames(CmdFilterOp filterOp) {
  auto& cs = cmds();
  Strings ret;
  for(const auto itr : cs)
    if (filterOp(itr.first)) ret.push_back(itr.first);
  return ret;
}

} // end namespace teditor
