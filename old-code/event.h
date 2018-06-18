#pragma once

#include <time.h>
#include "keys.h"
#include "terminal.h"
#include "buffer.h"
#include "utils.h"


namespace teditor {

namespace Tmp {
static const key_t Meta_Shift        = 2;
static const key_t Meta_Alt          = 3;
static const key_t Meta_AltShift     = 4;
static const key_t Meta_Ctrl         = 5;
static const key_t Meta_CtrlShift    = 6;
static const key_t Meta_AltCtrl      = 7;
static const key_t Meta_AltCtrlShift = 8;
static const key_t Meta_Motion       = 9; // for mouse events
static const key_t Meta_None         = 0;


enum Key {
    Key_F1             = (0xFFFF-0),
    Key_F2             = (0xFFFF-1),
    Key_F3             = (0xFFFF-2),
    Key_F4             = (0xFFFF-3),
    Key_F5             = (0xFFFF-4),
    Key_F6             = (0xFFFF-5),
    Key_F7             = (0xFFFF-6),
    Key_F8             = (0xFFFF-7),
    Key_F9             = (0xFFFF-8),
    Key_F10            = (0xFFFF-9),
    Key_F11            = (0xFFFF-10),
    Key_F12            = (0xFFFF-11),
    Key_Insert         = (0xFFFF-12),
    Key_Delete         = (0xFFFF-13),
    Key_Home           = (0xFFFF-14),
    Key_End            = (0xFFFF-15),
    Key_Pageup         = (0xFFFF-16),
    Key_PageDown       = (0xFFFF-17),
    Key_ArrowLeft      = (0xFFFF-18),
    Key_ArrowRight     = (0xFFFF-19),
    Key_ArrowDown      = (0xFFFF-20),
    Key_ArrowUp        = (0xFFFF-21),
    Key_MouseLeft      = (0xFFFF-22),
    Key_MouseRight     = (0xFFFF-23),
    Key_MouseMiddle    = (0xFFFF-24),
    Key_MouseRelease   = (0xFFFF-25),
    Key_MouseWheelUp   = (0xFFFF-26),
    Key_MouseWheelDown = (0xFFFF-27),
    Key_CtrlTilde       = 0x00,
    Key_Ctrl2           = 0x00, /* clash with 'CtrlTILDE' */
    Key_CtrlA           = 0x01,
    Key_CtrlB           = 0x02,
    Key_CtrlC           = 0x03,
    Key_CtrlD           = 0x04,
    Key_CtrlE           = 0x05,
    Key_CtrlF           = 0x06,
    Key_CtrlG           = 0x07,
    Key_Backspace       = 0x08,
    Key_CtrlH           = 0x08, /* clash with 'CtrlBACKSPACE' */
    Key_Tab             = 0x09,
    Key_CtrlI           = 0x09, /* clash with 'TAB' */
    Key_CtrlJ           = 0x0A,
    Key_CtrlK           = 0x0B,
    Key_CtrlL           = 0x0C,
    Key_Enter           = 0x0D,
    Key_CtrlM           = 0x0D, /* clash with 'ENTER' */
    Key_CtrlN           = 0x0E,
    Key_CtrlO           = 0x0F,
    Key_CtrlP           = 0x10,
    Key_CtrlQ           = 0x11,
    Key_CtrlR           = 0x12,
    Key_CtrlS           = 0x13,
    Key_CtrlT           = 0x14,
    Key_CtrlU           = 0x15,
    Key_CtrlV           = 0x16,
    Key_CtrlW           = 0x17,
    Key_CtrlX           = 0x18,
    Key_CtrlY           = 0x19,
    Key_CtrlZ           = 0x1A,
    Key_ESC             = 0x1B,
    Key_CtrlLsqBracket  = 0x1B, /* clash with 'ESC' */
    Key_Ctrl3           = 0x1B, /* clash with 'ESC' */
    Key_Ctrl4           = 0x1C,
    Key_CtrlBackslash   = 0x1C, /* clash with 'Ctrl4' */
    Key_Ctrl5           = 0x1D,
    Key_CtrlRsqBracket  = 0x1D, /* clash with 'Ctrl5' */
    Key_Ctrl6           = 0x1E,
    Key_Ctrl7           = 0x1F,
    Key_CtrlSlash       = 0x1F, /* clash with 'Ctrl7' */
    Key_CtrlUnderscore  = 0x1F, /* clash with 'Ctrl7' */
    Key_Space           = 0x20,
    Key_Backspace2      = 0x7F,
    Key_Ctrl8           = 0x7F, /* clash with 'BACKSPACE2' */
    Key_None            = 0xFFFFFFFF,
    Key_Invalid         = 0x0
};
}; // end namespace Tmp

struct Click {
    Key type;
    struct timespec ts;
    bool isDouble;

    static int clickCount;

    Click(Key type_);
    bool isDoubleClick() const { return isDouble; }

private:
    static Click lastClick;
    static double DoubleClickTime;
};


class Editor;

struct Event {
    EventType type;
    Pos2d<uint16_t> loc;
    uint32_t meta;
    Key key;
    Chr chr;
    bool cutesc;

    Event();
    void reset();
    int waitAndFill(struct timeval* timeout, Editor& tb);
    bool asciiInput() const;

private:
    void decodeChar(uint32_t ch);
    int decodeUtf8(int inout, char c);
    int decodeEsc(int inout, const Terminal& term);
    int parseMouseEvent(char* buf, int len);
    int parseEscSeq(char* seq, int len, const Terminal& term);
    int parseBracketEsc(char* seq, int len);
    int readAndExtract(int inout, const Terminal& term);
};

}; // end namespace teditor
