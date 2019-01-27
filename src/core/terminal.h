#pragma once

#include <string>
#include <vector>
#include "utils.h"
#include <string.h>
#include <termios.h>
#include "pos2d.h"


namespace teditor {

// [0m sgr0   Reset all attributes
// [1m bold   Set "bright" attribute
// [2m dim    Set "dim" attribute
// [3m smso   Set "standout" attribute
// [4m set    Set "underscore" (underlined text) attribute
// [5m blink  Set "blink" attribute
// [7m rev    Set "reverse" attribute
// [8m invis  Set "hidden" attribute
enum Func {
    Func_EnterCA,
    Func_ExitCA,
    Func_ShowCursor,
    Func_HideCursor,
    Func_ClearScreen,
    Func_Sgr0,
    Func_Underline,
    Func_Bold,
    Func_Blink,
    Func_Reverse,
    Func_EnterKeypad,
    Func_ExitKeypad,
    Func_EnterMouse,
    Func_ExitMouse,
    Func_FuncsNum
};


class Terminal {
public:
    Terminal(const std::string& tty);
    ~Terminal();
    int getFd() const { return inout; }
    std::string name() const { return termName; }
    int width() const { return tsize.x; }
    int height() const { return tsize.y; }

    /**
     * @defgroup PtyOps Operations to interact with the underlying pty
     * @{
     */
    /**
     * @brief append data at the end of the current buffer contents
     * @param data the data to be appended
     * @param len number of elements in the data
     */
    void puts(const char* data, size_t len);
    void puts(const char* data) { puts(data, strlen(data)); }
    void puts(const std::string& data) { puts(data.c_str(), data.length()); }
    void puts(Func f) { puts(func(f)); }
    /** flush the contents of the buffer to the pty */
    void flush();
    /** update the terminal size */
    void updateTermSize();
    /** @} */

    static const int Magic;
    static const int TiFuncs[];
    static const int TiNFuncs;
    static const int TiKeys[];
    static const int TiNKeys;

private:
    /** keys and functions */
    Strings keys, funcs;
    /** pty name */
    std::string termName;
    /** buffer used to communicate commands to the pty */
    std::string outbuff;
    /** tty file to be used for communication */
    std::string ttyFile;
    /** file descriptor for the ttyFile */
    int inout;
    /** termios struct to update attrs */
    struct termios tios, origTios;
    /** terminal size */
    Pos2di tsize;

    static const std::string EnterMouseSeq;
    static const std::string ExitMouseSeq;
    static const int BuffSize;

    enum ColorSupport {
        CS_None = 0,
        CS_256,
        CS_True
    };

    void setupTios();
    ColorSupport colorSupported() const;
    const char* key(int id) const { return keys[id].c_str(); }
    const char* func(int id) const { return funcs[id].c_str(); }
    std::string tryReading(const char* path, const char* term) const;
    std::string loadTerminfo() const;
    std::string copyString(const std::string& tidata, int str, int table) const;
};

}; // end namespace teditor
