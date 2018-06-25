#pragma once

#include "key_cmd_map.h"
#include "colors.h"
#include <string>


namespace teditor {

/** Mode attached with a buffer */
struct Mode {
    KeyCmdMap kcMap;
    ColorMap cMap;
    std::string word;
    std::string name;
};


/** the default (catch-all) mode */
void textMode(Mode& m);

/** mode while loading and working with dirs */
void dirMode(Mode& m);

/** mode for the cmd-msg-bar */
void cmBarMode(Mode& m);

}; // end namespace teditor
