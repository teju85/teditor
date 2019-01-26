#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include "utils.h"
#include <utility>


namespace teditor {

class Editor;


/** main lambda to operate on the editor/buffer */
typedef void (*OperateFunc)(Editor& ed);
/** help message associated with this command */
typedef std::string (*HelpFunc)(Editor& ed);
/** Main class to add operators onto the editor/buffer */
typedef std::pair<OperateFunc, HelpFunc> Command;
/** list of all currently registered commands */
typedef std::unordered_map<std::string,Command> CommandMap;


/**
 * @defgroup CommandFunctions All methods related to creating/querying commands
 * @{
 */
/** Accessor function to the command map */
CommandMap& cmds();
/**
 * @brief helper to register a named command
 * @param name name of the command (useful for later queries)
 * @param cmd the command object
 */
void registerCmd(const std::string& name, const Command& cmd);
/**
 * @brief Helper to return the command of interest
 * @param cmd name of the command
 * @return the command object
 */
const Command& getCmd(const std::string& cmd);
/** Accessor function to the list of command names */
Strings allCmdNames();
/** @} */


/** helper class to register the command into the command database */
struct RegisterCmd {
    RegisterCmd(const std::string& cmd, OperateFunc op, HelpFunc help=nullptr) {
        registerCmd(cmd, std::make_pair(op, help));
    }
};

/** macro to register a command to the database */
#define DEF_CMD(UniqName, CmdName, OpFunc, HelpFunc)    \
    RegisterCmd cmd ## UniqName(CmdName, OpFunc, HelpFunc)

/** helper macro to define a command without any help */
#define DEF_CMD_NO_HELP(UniqName, CmdName, OpFunc)      \
    DEF_CMD(UniqName, CmdName, OpFunc, nullptr)

/** helper macro to define a command function */
#define DEF_OP()  [](Editor& ed)

/** helper macro to define a help function */
#define DEF_HELP()  [](Editor& ed) -> std::string

}; // end namespace teditor
