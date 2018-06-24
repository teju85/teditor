#pragma once

#include <string>
#include <vector>
#include "utils.h"

// [0m sgr0   Reset all attributes
// [1m bold   Set "bright" attribute
// [2m dim    Set "dim" attribute
// [3m smso   Set "standout" attribute
// [4m set    Set "underscore" (underlined text) attribute
// [5m blink  Set "blink" attribute
// [7m rev    Set "reverse" attribute
// [8m invis  Set "hidden" attribute

namespace teditor {

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


enum ColorSupport {
    ColorSupport_None = 0,
    ColorSupport_256,
    ColorSupport_True
};


class Terminal {
public:
    Terminal();
    ColorSupport colorSupported() const;
    std::string name() const { return termName; }
    const char* key(int id) const { return keys[id].c_str(); }
    const char* func(int id) const { return funcs[id].c_str(); }

    static const int Magic;
    static const int TiFuncs[];
    static const int TiNFuncs;
    static const int TiKeys[];
    static const int TiNKeys;

private:
    Strings keys, funcs;
    std::string termName;

    static const std::string EnterMouseSeq;
    static const std::string ExitMouseSeq;

    std::string tryReading(const char* path, const char* term) const;
    std::string loadTerminfo() const;
    std::string copyString(const std::string& tidata, int str, int table) const;
};

}; // end namespace teditor
