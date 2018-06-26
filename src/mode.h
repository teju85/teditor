#pragma once

#include "key_cmd_map.h"
#include "colors.h"
#include <string>


namespace teditor {

class Buffer;

/** Functor to apply indentation on a given line in the buffer */
typedef std::string(*Indentor)(int, Buffer&);


/** Mode attached with a buffer */
struct Mode {
    /** key map to be used for the buffer this mode applies to */
    KeyCmdMap kcMap;
    /** color map to be used for the buffer this mode applies to */
    ColorMap cMap;
    /** list a characters that define a word */
    std::string word;
    /** mode name */
    std::string name;
    /** the indentation function */
    Indentor indent;
};


/** the default (catch-all) mode */
void textMode(Mode& m);

/** mode while loading and working with dirs */
void dirMode(Mode& m);

/** mode for the cmd-msg-bar */
void cmBarMode(Mode& m);

}; // end namespace teditor
