#include "command.h"
#include "utils.h"


namespace teditor {

CmdCreatorMap& cmds() {
    static CmdCreatorMap _cmds;
    return _cmds;
}

void Command::registerCmd(const std::string& cmd, CmdCreator fptr) {
    auto& c = cmds();
    ASSERT(c.find(cmd) == c.end(),
           "Command '%s' already registered!", cmd.c_str());
    c[cmd] = fptr;
}

CmdPtr Command::createCmd(const std::string& cmd) {
    auto& c = cmds();
    auto itr = c.find(cmd);
    ASSERT(itr != c.end(), "Command '%s' not registered!", cmd.c_str());
    CmdPtr ptr(itr->second());
    return ptr;
}

} // end namespace teditor
