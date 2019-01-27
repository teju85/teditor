#pragma once

#include <stdint.h>
#include "utils.h"


namespace teditor {

typedef uint32_t key_t;


static const key_t Meta_None         = 0x00000000;
static const key_t Meta_Shift        = 0x00010000;
static const key_t Meta_Alt          = 0x00020000;
static const key_t Meta_Ctrl         = 0x00040000;
static const key_t Meta_Motion       = 0x00080000; // for mouse events

static const key_t Key_F1             = (0xFFFF-0);
static const key_t Key_F2             = (0xFFFF-1);
static const key_t Key_F3             = (0xFFFF-2);
static const key_t Key_F4             = (0xFFFF-3);
static const key_t Key_F5             = (0xFFFF-4);
static const key_t Key_F6             = (0xFFFF-5);
static const key_t Key_F7             = (0xFFFF-6);
static const key_t Key_F8             = (0xFFFF-7);
static const key_t Key_F9             = (0xFFFF-8);
static const key_t Key_F10            = (0xFFFF-9);
static const key_t Key_F11            = (0xFFFF-10);
static const key_t Key_F12            = (0xFFFF-11);
static const key_t Key_Insert         = (0xFFFF-12);
static const key_t Key_Delete         = (0xFFFF-13);
static const key_t Key_Home           = (0xFFFF-14);
static const key_t Key_End            = (0xFFFF-15);
static const key_t Key_PageUp         = (0xFFFF-16);
static const key_t Key_PageDown       = (0xFFFF-17);
static const key_t Key_ArrowLeft      = (0xFFFF-18);
static const key_t Key_ArrowRight     = (0xFFFF-19);
static const key_t Key_ArrowDown      = (0xFFFF-20);
static const key_t Key_ArrowUp        = (0xFFFF-21);
static const key_t Key_MouseLeft      = (0xFFFF-22);
static const key_t Key_MouseRight     = (0xFFFF-23);
static const key_t Key_MouseMiddle    = (0xFFFF-24);
static const key_t Key_MouseRelease   = (0xFFFF-25);
static const key_t Key_MouseWheelUp   = (0xFFFF-26);
static const key_t Key_MouseWheelDown = (0xFFFF-27);
static const key_t Key_CtrlTilde       = 0x00;
static const key_t Key_Ctrl2           = 0x00; /* clash with 'CtrlTilde' */
static const key_t Key_CtrlA           = 0x01;
static const key_t Key_CtrlB           = 0x02;
static const key_t Key_CtrlC           = 0x03;
static const key_t Key_CtrlD           = 0x04;
static const key_t Key_CtrlE           = 0x05;
static const key_t Key_CtrlF           = 0x06;
static const key_t Key_CtrlG           = 0x07;
static const key_t Key_Backspace       = 0x08;
static const key_t Key_CtrlH           = 0x08; /* clash with 'Backspace' */
static const key_t Key_Tab             = 0x09;
static const key_t Key_CtrlI           = 0x09; /* clash with 'Tab' */
static const key_t Key_CtrlJ           = 0x0A;
static const key_t Key_CtrlK           = 0x0B;
static const key_t Key_CtrlL           = 0x0C;
static const key_t Key_Enter           = 0x0D;
static const key_t Key_CtrlM           = 0x0D; /* clash with 'Enter' */
static const key_t Key_CtrlN           = 0x0E;
static const key_t Key_CtrlO           = 0x0F;
static const key_t Key_CtrlP           = 0x10;
static const key_t Key_CtrlQ           = 0x11;
static const key_t Key_CtrlR           = 0x12;
static const key_t Key_CtrlS           = 0x13;
static const key_t Key_CtrlT           = 0x14;
static const key_t Key_CtrlU           = 0x15;
static const key_t Key_CtrlV           = 0x16;
static const key_t Key_CtrlW           = 0x17;
static const key_t Key_CtrlX           = 0x18;
static const key_t Key_CtrlY           = 0x19;
static const key_t Key_CtrlZ           = 0x1A;
static const key_t Key_Esc             = 0x1B;
static const key_t Key_CtrlLsqBracket  = 0x1B; /* clash with 'Esc' */
static const key_t Key_Ctrl3           = 0x1B; /* clash with 'Esc' */
static const key_t Key_Ctrl4           = 0x1C;
static const key_t Key_CtrlBackslash   = 0x1C; /* clash with 'Ctrl4' */
static const key_t Key_Ctrl5           = 0x1D;
static const key_t Key_CtrlRsqBracket  = 0x1D; /* clash with 'Ctrl5' */
static const key_t Key_Ctrl6           = 0x1E;
static const key_t Key_Ctrl7           = 0x1F;
static const key_t Key_CtrlSlash       = 0x1F; /* clash with 'Ctrl7' */
static const key_t Key_CtrlUnderscore  = 0x1F; /* clash with 'Ctrl7' */
static const key_t Key_Space           = (key_t)' ';
static const key_t Key_Exclaim         = (key_t)'!';
static const key_t Key_DQuote          = (key_t)'"';
static const key_t Key_Hash            = (key_t)'#';
static const key_t Key_Dollar          = (key_t)'$';
static const key_t Key_Percent         = (key_t)'%';
static const key_t Key_Ampersand       = (key_t)'&';
static const key_t Key_SQuote          = (key_t)'\'';
static const key_t Key_BrktOpen        = (key_t)'(';
static const key_t Key_BrktClose       = (key_t)')';
static const key_t Key_Star            = (key_t)'*';
static const key_t Key_Plus            = (key_t)'+';
static const key_t Key_Comma           = (key_t)',';
static const key_t Key_Minus           = (key_t)'-';
static const key_t Key_Dot             = (key_t)'.';
static const key_t Key_Slash           = (key_t)'/';
static const key_t Key_Zero            = (key_t)'0';
static const key_t Key_One             = (key_t)'1';
static const key_t Key_Two             = (key_t)'2';
static const key_t Key_Three           = (key_t)'3';
static const key_t Key_Four            = (key_t)'4';
static const key_t Key_Five            = (key_t)'5';
static const key_t Key_Six             = (key_t)'6';
static const key_t Key_Seven           = (key_t)'7';
static const key_t Key_Eight           = (key_t)'8';
static const key_t Key_Nine            = (key_t)'9';
static const key_t Key_Colon           = (key_t)':';
static const key_t Key_SemiColo        = (key_t)';';
static const key_t Key_Lt              = (key_t)'<';
static const key_t Key_Question        = (key_t)'?';
static const key_t Key_At              = (key_t)'@';
static const key_t Key_A               = (key_t)'A';
static const key_t Key_B               = (key_t)'B';
static const key_t Key_C               = (key_t)'C';
static const key_t Key_D               = (key_t)'D';
static const key_t Key_E               = (key_t)'E';
static const key_t Key_F               = (key_t)'F';
static const key_t Key_G               = (key_t)'G';
static const key_t Key_H               = (key_t)'H';
static const key_t Key_I               = (key_t)'I';
static const key_t Key_J               = (key_t)'J';
static const key_t Key_K               = (key_t)'K';
static const key_t Key_L               = (key_t)'L';
static const key_t Key_M               = (key_t)'M';
static const key_t Key_N               = (key_t)'N';
static const key_t Key_O               = (key_t)'O';
static const key_t Key_P               = (key_t)'P';
static const key_t Key_Q               = (key_t)'Q';
static const key_t Key_R               = (key_t)'R';
static const key_t Key_S               = (key_t)'S';
static const key_t Key_T               = (key_t)'T';
static const key_t Key_U               = (key_t)'U';
static const key_t Key_V               = (key_t)'V';
static const key_t Key_W               = (key_t)'W';
static const key_t Key_X               = (key_t)'X';
static const key_t Key_Y               = (key_t)'Y';
static const key_t Key_Z               = (key_t)'Z';
static const key_t Key_SqBrktOpt       = (key_t)'[';
static const key_t Key_BackSlash       = (key_t)'\\';
static const key_t Key_SqBrktClose     = (key_t)']';
static const key_t Key_Caret           = (key_t)'^';
static const key_t Key_Underscore      = (key_t)'_';
static const key_t Key_Backtick        = (key_t)'`';
static const key_t Key_a               = (key_t)'a';
static const key_t Key_b               = (key_t)'b';
static const key_t Key_c               = (key_t)'c';
static const key_t Key_d               = (key_t)'d';
static const key_t Key_e               = (key_t)'e';
static const key_t Key_f               = (key_t)'f';
static const key_t Key_g               = (key_t)'g';
static const key_t Key_h               = (key_t)'h';
static const key_t Key_i               = (key_t)'i';
static const key_t Key_j               = (key_t)'j';
static const key_t Key_k               = (key_t)'k';
static const key_t Key_l               = (key_t)'l';
static const key_t Key_m               = (key_t)'m';
static const key_t Key_n               = (key_t)'n';
static const key_t Key_o               = (key_t)'o';
static const key_t Key_p               = (key_t)'p';
static const key_t Key_q               = (key_t)'q';
static const key_t Key_r               = (key_t)'r';
static const key_t Key_s               = (key_t)'s';
static const key_t Key_t               = (key_t)'t';
static const key_t Key_u               = (key_t)'u';
static const key_t Key_v               = (key_t)'v';
static const key_t Key_w               = (key_t)'w';
static const key_t Key_x               = (key_t)'x';
static const key_t Key_y               = (key_t)'y';
static const key_t Key_z               = (key_t)'z';
static const key_t Key_FBrktOpen       = (key_t)'{';
static const key_t Key_Line            = (key_t)'|';
static const key_t Key_FBrktClose      = (key_t)'}';
static const key_t Key_Tilde           = (key_t)'~';
static const key_t Key_Backspace2      = 0x7F;
static const key_t Key_Ctrl8           = 0x7F; /* clash with 'Backspace2' */
static const key_t Key_Invalid         = 0xFFFFFFFF;


struct MetaKey {
    MetaKey(): key(Key_Invalid) {}
    MetaKey(key_t k): key(k) {}
    MetaKey(key_t m, key_t k): key(m | k) {}
    bool isInvalid() const { return key == Key_Invalid; }
    key_t getMeta() const { return key & ~Mask; }
    key_t getKey() const { return key & Mask; }
    key_t getFull() const { return key; }
    bool isAscii() const;
    void reset() { key = Key_Invalid; }
    void setKey(key_t k) { key = getMeta() | (k & Mask); }
    void setMeta(key_t m) { key = (m & ~Mask) | getKey(); }
    void updateMeta(key_t m) { key = (m & ~Mask) | key; }
    std::string toStr() const;

private:
    key_t key;

    static const uint32_t Mask;
};

bool operator==(const MetaKey& mk1, const MetaKey& mk2);

}; // end namespace teditor
