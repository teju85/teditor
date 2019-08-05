#include "command.h"
#include "editor.h"
#include "utils.h"


namespace teditor {

CommandMap& cmds() {
  static CommandMap _cmds;
  return _cmds;
}

void registerCmd(const std::string& name, const Command& cmd) {
  auto& c = cmds();
  ASSERT(c.find(name) == c.end(),
         "Command '%s' already registered!", name.c_str());
  c[name] = cmd;
}

const Command& getCmd(const std::string& cmd) {
  auto& c = cmds();
  const auto itr = c.find(cmd);
  ASSERT(itr != c.end(), "Command '%s' not registered!", cmd.c_str());
  return itr->second;
}

Strings allCmdNames() {
  auto& cs = cmds();
  Strings ret;
  for(const auto itr : cs) ret.push_back(itr.first);
  return ret;
}

} // end namespace teditor
