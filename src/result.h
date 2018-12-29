#pragma once

#include "utils.h"
#include <string>


namespace teditor {

/**
 * @brief This represents the result of insertion/deletion operations which
 * occur in a buffer via multiple commands
 */
class Result {
public:
    /** ctor */
    Result();

private:
    /** name of the command which resulted in these chars */
    std::string name;
    /** characters inserted/deleted */
    Strings chars;
    /** starting position of the command before execution */
    Positions startPos;
    /** current position after the command execution */
    Positions lastPos;
};

} // end namespace teditor
