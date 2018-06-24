#pragma once

#include "keys.h"
#include <vector>
#include <string>


namespace teditor {

enum EventType {
    Event_Key = 1,
    Event_Resize,
    Event_Mouse,
    Event_None = 0
};


class Editor;

///@todo: implement decodeUtf8 if needed
///@todo: add support for mouse in future
struct Input {
    EventType type;
    MetaKey mk;
    Pos2d<uint16_t> loc; // for mouse events

    Input();
    void reset();
    bool asciiInput() const { return mk.isAscii(); }
    int readAndExtract(int inout);
    int waitAndFill(struct timeval* timeout, Editor& tb);
    const std::string& getOldSeq() const { return oldSeq; }

    /** flag to raise an undefined escape sequence scenario */
    static const int UndefinedSequence;

private:
    std::string seq, oldSeq;

    int decodeChar(key_t ch);
    int decodeEscSeq();
};

}; // end namespace teditor
