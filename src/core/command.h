#pragma once

#include <string>
#include <unordered_map>
#include "utils.h"
#include <utility>

namespace teditor {

class Editor;


/** main lambda to operate on the editor/buffer */
typedef void (*OperateFunc)(Editor& ed);
/** Main class to add operators onto the editor/buffer */
typedef std::pair<OperateFunc, std::string> Command;
/** list of all currently registered commands */
typedef std::unordered_map<std::string, Command> CommandMap;
/** command name filter */
typedef bool (*CmdFilterOp)(const std::string&);

/** default command name filter */
bool defaultCmdFilterOp(const std::string& name);


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
 * @param filterOp filter to be applied on the command names. If the filter
 *                 returns a true for a given command name, it will be added
 *                 in the output.
 * @return command names which pass the filter test */
Strings allCmdNames(CmdFilterOp filterOp = defaultCmdFilterOp);
/** @} */


struct CmdRegistrar {
  CmdRegistrar(const std::string& cmd, OperateFunc op, const std::string& grp);
};

/** macro to register a command to the database */
#define DEF_CMD(UniqName, CmdName, CmdGroup, OpFunc)    \
  CmdRegistrar cmd ## UniqName(CmdName, OpFunc, CmdGroup)

/** helper macro to define a command function */
#define DEF_OP()  [](Editor& ed)

}; // end namespace teditor
