#pragma once

#include "utils.h"
#include <string>
#include "pos2d.h"


namespace teditor {

/**
 * @brief This represents the result of insertion/deletion operations which
 * occur in a buffer via multiple commands
 */
class Result {
public:
    /** ctor */
    Result();

    /**
     * @brief Characters added by the current command to the current buffer
     * @param c the name of the command which added these characters
     * @param s characters added
     * @param loc location where these chars were added
     * @return true if this could be added to the current object, else false
     */
    //bool added(const std::string& c, const Strings& s, const Positions& loc);

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
