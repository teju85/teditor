#include "input.h"
#include <unordered_map>
#include <vector>
#include <unistd.h>
#include "editor.h"
#include "logger.h"


namespace teditor {

struct KeyCombo {
    MetaKey mk;
    std::string escSeq;
    static const std::vector<KeyCombo> Combos;
};

struct AllCombos {
    AllCombos();
    static std::unordered_map<std::string, MetaKey> allKeys;
    static const AllCombos test;
};


#include "custom/xterm_strokes.h"


std::unordered_map<std::string, MetaKey> AllCombos::allKeys;
const AllCombos AllCombos::test;

AllCombos::AllCombos() {
    for(auto& c : KeyCombo::Combos)
        allKeys[c.escSeq] = c.mk;
}


const int Input::UndefinedSequence = -10;

Input::Input() {
    reset();
    seq.clear();
    oldSeq.clear();
}

void Input::reset() {
    type = Event_None;
    loc = {0, 0};
    mk.reset();
    mk.setMeta(Meta_None);
}

int Input::decodeChar(key_t ch) {
    mk.setKey(ch);
    // Ctrl A-Z and 0-7, except Enter
    if(ch < Key_Space && ch != Key_Enter && ch != Key_Tab)
        mk.updateMeta(Meta_Ctrl);
    return 1;
}

int Input::decodeEscSeq() {
    int len = (int)seq.length();
    DEBUG("decodeEscSeq: len=%d seq=%s\n", len, seq.c_str());
    if(len == 0) {
        mk.setKey(Key_Esc);
        return 1;
    }
    // Alt-<key>
    if(len == 1 && (char)Key_Space <= seq[0] && seq[0] <= (char)Key_Tilde) {
        mk = MetaKey(Meta_Alt, (key_t)seq[0]);
        seq.erase(0, 1);
        return 1;
    }
    // check for shortest string matching the list of keys
    for(int i=1;i<=len;++i) {
        std::string str(seq, 0, i);
        const auto itr = AllCombos::allKeys.find(str);
        if(itr != AllCombos::allKeys.end()) {
            mk = itr->second;
            seq.erase(0, i);
            return i;
        }
    }
    oldSeq = seq;
    seq.clear();
    return UndefinedSequence;
}

int Input::readAndExtract(int inout) {
    if(seq.empty()) {
        int rs = 1;
        do {
            char c;
            rs = read(inout, &c, 1);
            if(rs > 0)
                seq += c;
        } while(rs > 0);
    }
    ///@todo: support for mouse events
    type = Event_Key;
    mk.setMeta(Meta_None);
    char c = seq[0];
    seq.erase(0, 1);
    if(c == (char)Key_Esc)
        return decodeEscSeq();
    if(c <= (char)Key_Backspace2)
        return decodeChar(c);
    ASSERT(false, "Input::readAndExtract: UTF8 sequence found '%u'!",
           (uint32_t)c);
}

int Input::waitAndFill(struct timeval* timeout, Editor& tb) {
    int inout = tb.getFd();
    int winchFds[2] = {tb.getWinchFd(0), tb.getWinchFd(1)};
    int n;
    fd_set events;
    reset();
    while(1) {
        FD_ZERO(&events);
        FD_SET(inout, &events);
        FD_SET(winchFds[0], &events);
        int maxfd  = std::max(winchFds[0], inout);
        int result = select(maxfd+1, &events, 0, 0, timeout);
        if(!result)
            return 0;
        // resize event
        if(FD_ISSET(winchFds[0], &events)) {
            type = Event_Resize;
            int zzz = 0;
            n = read(winchFds[0], &zzz, sizeof(int));
            tb.requestResize();
            return type;
        } else {
            tb.disableResize();
        }
        // key/mouse events
        if(FD_ISSET(inout, &events)) {
            n = readAndExtract(inout);
            if(n == UndefinedSequence)
                return n;
            if(n < 0)
                return -1;
            if(n > 0)
                return type;
        }
    }
}

} // end namespace teditor
