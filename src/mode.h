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
};


/** the default (catch-all) mode */
Mode textMode();

/** mode while loading and working with dirs */
Mode dirMode();

}; // end namespace teditor
