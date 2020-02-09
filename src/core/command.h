#pragma once

#include <string>
#include <unordered_map>
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
/**
 * @brief Helper to return the command of interest
 * @param cmd name of the command
 * @return the command object
 */
const Command& getCmd(const std::string& cmd);
/**
 * @brief Accessor function to the list of command names
 * @param mn mode name
 * @return all command names that have been registered under this mode */
Strings allCmdNames(const std::string& mn="");
/** @} */


struct CmdRegistrar {
  CmdRegistrar(const std::string& cmd, OperateFunc op, HelpFunc help);
};

/** macro to register a command to the database */
#define DEF_CMD(UniqName, CmdName, OpFunc, HelpFunc)            \
  CmdRegistrar cmd ## UniqName(CmdName, OpFunc, HelpFunc)

/** helper macro to define a command function */
#define DEF_OP()  [](Editor& ed)

/** helper macro to define a help function */
#define DEF_HELP()  [](Editor& ed) -> std::string

}; // end namespace teditor
