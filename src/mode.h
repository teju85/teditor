#pragma once

#include "key_cmd_map.h"
#include "colors.h"
#include <string>
#include <memory>


namespace teditor {

class Buffer;


/** Apply indentation on a given line in the buffer */
class Indentor {
public:
    Indentor(int i): indentSize(i) {}
    virtual ~Indentor() {}
    virtual int indent(Buffer& buf, int line);

protected:
    /** return indent size */
    int getIndentSize() const { return indentSize; }

private:
    /** indentation length (in spaces) */
    int indentSize;
};


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
    std::shared_ptr<Indentor> indent;
};


/** the default (catch-all) mode */
void textMode(Mode& m);

/** mode while loading and working with dirs */
void dirMode(Mode& m);

/** mode for the cmd-msg-bar */
void cmBarMode(Mode& m);

/** mode for the c/cpp/cuda files */
void cppMode(Mode& m);

}; // end namespace teditor
