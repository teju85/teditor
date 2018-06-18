#include "event.h"
#include "utf8.h"
#include <unistd.h>
#include "utils.h"
#include "editor.h"
#include "logger.h"
#include <ctype.h>

namespace teditor {

Click Click::lastClick(Key_None);
int Click::clickCount = 0;
double Click::DoubleClickTime = 0.4;


Click::Click(Key type_): type(type_), ts({0, 0}), isDouble(false) {
    clock_gettime(CLOCK_MONOTONIC, &ts);
    struct timespec start = lastClick.ts;
    struct timespec end = ts;
    double timeDiff = ((double)end.tv_sec + 1.0e-9 * end.tv_nsec);
    timeDiff -= ((double)start.tv_sec + 1.0e-9 * start.tv_nsec);
    isDouble = (timeDiff < DoubleClickTime) && (lastClick.type == type);
    lastClick = *this;
}


static const int MaxSeq = 16;
static char seq[MaxSeq] = "";

Event::Event() {
    cutesc = false;
    reset();
}

bool Event::asciiInput() const {
    return isprint(chr) && meta == Meta_None;
}

void Event::reset() {
    type = Event_None;
    loc = {0, 0};
    meta = Meta_None;
    key = Key_Invalid;
    chr = 0;
}

void Event::decodeChar(uint32_t ch) {
    if(ch == 127) {
        key = Key_Backspace2;
    } else if(ch < 32) { // ctrl + a-z or number up to 7
        meta = ch == 13? Meta_None : Meta_Ctrl;
        key = (Key)ch;
    } else { // a-z -- A-Z -- 0-9
        meta = Meta_None;
        chr = (char)ch;
    }
}

int Event::decodeUtf8(int inout, char c) {
    int len = (int)Utf8::charLen(c);
    seq[0] = c;
    int nread = 1;
    while(nread < len) {
        // if read error, or if didn't read end of sequence, return -1
        int rs = read(inout, seq + nread++, 1);
        if(rs < 1)
            return -1;
    }
    seq[nread] = '\0';
    uint32_t out;
    int outlen = Utf8::char2unicode(&out, seq);
    decodeChar(out);
    return outlen;
}

int Event::decodeEsc(int inout, const Terminal& term) {
    seq[0] = 27;
    int nread = 1;
    DEBUG("decodeEsc: at start seq[0]=%d nread=%d\n", seq[0], nread);
    while(nread < MaxSeq) {
        int rs = read(inout, seq+nread, 1);
        DEBUG("decodeEsc: read rs=%d nread=%d str+nread=%d\n", rs, nread,
              seq[nread]);
        if(rs <= 0)
            break;
        nread++;
        // handle urxvt alt + keys
        if(seq[nread-1] == 27) { // found another escape char!
            if(seq[nread-2] == 27) { // double esc
                // end of the road, so it's alt+esc
                if(read(inout, seq + nread++, 1) == 0) {
                    DEBUG("decodeEsc: read double-esc rs=%d nread=%d str+nread=%d\n",
                          rs, nread, seq[nread]);
                    key = Key_ESC;
                    meta = Meta_Alt;
                    return 1;
                }
            } else { // if not end of road, it must be ^[^[[A (urxvt alt+arrows)
                cutesc = true;
                break;
            }
        }
    }
    if(nread == MaxSeq)
        return 0;
    seq[nread] = '\0';
    DEBUG("decodeEsc: after reading: nread=%d\n", nread);
    int len = parseMouseEvent(seq, nread);
    if(len > 0) {
        DEBUG("decodeEsc: mouse event found len=%d\n", len);
        return len;
    }
    return parseEscSeq(seq, nread, term);
}

int Event::parseMouseEvent(char* buf, int len) {
    if((len >= 6) && startsWith(buf, len, "\033[M")) {
        // X10 mouse encoding, the simplest one
        // \033 [ M Cb Cx Cy
        int b = buf[3] - 32;
        switch(b & 3) {
        case 0:
            key = ((b & 64) != 0)? Key_MouseWheelUp : Key_MouseLeft;
            break;
        case 1:
            key = ((b & 64) != 0)? Key_MouseWheelDown : Key_MouseMiddle;
            break;
        case 2:
            key = Key_MouseRight;
            break;
        case 3:
            key = Key_MouseRelease;
            break;
        default:
            return -6;
        }
        type = Event_Mouse;
        if((b & 32) != 0)
            meta |= Meta_Motion;
        // the coord is 1,1 for upper left
        loc.x = (uint8_t)buf[4] - 1 - 32;
        loc.y = (uint8_t)buf[5] - 1 - 32;
        if((key > Key_MouseRelease) && !(meta & Meta_Motion)) { // click
            Click click(key);
            if(click.isDoubleClick()) {
                Click::clickCount++;
            } else {
                Click::clickCount = 1;
            }
        }
        return 6;
    } else if(startsWith(buf, len, "\033[<") || startsWith(buf, len, "\033[")) {
        // xterm 1006 extended mode or urxvt 1015 extended mode
        // xterm: \033 [ < Cb ; Cx ; Cy (M or m)
        // urxvt: \033 [ Cb ; Cx ; Cy M
        int mi = -1, starti = -1;
        int isM, isU, s1 = -1, s2 = -1;
        int n1 = 0, n2 = 0, n3 = 0;
        for(int i=0;i<len;i++) {
            // We search the first (s1) and the last (s2) ';'
            if (buf[i] == ';') {
                if (s1 == -1) s1 = i;
                s2 = i;
            }
            // We search for the first 'm' or 'M'
            if ((buf[i] == 'm' || buf[i] == 'M') && mi == -1) {
                mi = i;
                break;
            }
        }
        if(mi == -1)
            return 0;
        // whether it's a capital M or not
        isM = (buf[mi] == 'M');
        if(buf[2] == '<') {
            isU = 0;
            starti = 3;
        } else {
            isU = 1;
            starti = 2;
        }
        if(s1 == -1 || s2 == -1 || s1 == s2)
            return 0;
        n1 = strtoul(&buf[starti], NULL, 10);
        n2 = strtoul(&buf[s1 + 1], NULL, 10);
        n3 = strtoul(&buf[s2 + 1], NULL, 10);
        if(isU)
            n1 -= 32;
        switch (n1 & 3) {
        case 0:
            key = ((n1&64) != 0)? Key_MouseWheelUp : Key_MouseLeft;
            break;
        case 1:
            key = ((n1&64) != 0)? Key_MouseWheelDown : Key_MouseMiddle;
            break;
        case 2:
            key = Key_MouseRight;
            break;
        case 3:
            key = Key_MouseRelease;
            break;
        default:
            return mi + 1;
        }
        if(!isM) // on xterm mouse release is signaled by lowercase m
            key = Key_MouseRelease;
        type = Event_Mouse;
        if((n1 & 32) != 0)
            meta |= Meta_Motion;
        loc.x = (uint8_t)n2 - 1;
        loc.y = (uint8_t)n3 - 1;
        if((key > Key_MouseRelease) && !(meta & Meta_Motion)) { // click
            Click click(key);
            if(click.isDoubleClick()) {
                Click::clickCount++;
            } else {
                Click::clickCount = 1;
            }
        }
        return mi + 1;
    }
    return 0;
}

int Event::parseBracketEsc(char* seq, int len) {
    int last = seq[len-1];
    int res  = 0;
    if(len == 3) {
        if(last == 'Z') { // shift + tab
            meta = Meta_Shift;
            key = Key_Tab;
        } else if (('a' <= last) && (last <= 'd')) { // mrxvt shift + left/right or ctrl+shift + up/down
            // TODO: handle ctrl + shift + arrow in mrxvt
            meta = Meta_Shift;
            key = (Key)(0xFFFF + (last - 118));
        } else {
            res = -1;
        }
    } else if(len > 5) { // xterm shift or control + f1/keys/arrows
        if(last == '~') {
            if(seq[3] == ';') { // xfce shift/ctrl + keys (delete, pageup/down)
                int offset = seq[2] < 53 ? 38 : seq[2] > 54 ? 41 : 37;
                key = (Key)(0xFFFF - (seq[2] - offset));
                meta = seq[4] - 48;
            } else {
                return -1;
            }
        } else if(('A' <= last) && (last <= 'Z')) {
            meta = seq[4] - 48;
            if(last >= 80) { // f1-f4 xterm
                // event->key = last - 69; // TODO: verify
                key = (Key)(0xFFFF + (last - 86));
            } else {  // ctrl + arrows urxvt or shift+home/end in xfce4-term
                int offset = last == 70 ? 85 : 86; // handle shift+end offset in xfce4
                key = (Key)(0xFFFF + (last - offset));
            }
        } else {
            res = -1;
        }
    } else if((last == '^') || (last == '@') || (last == '$')) { // 4 or 5 in length
        if(('1' <= seq[2]) && (seq[2] <= '8')) { // ctrl/shift + keys urxvt
            // 50 ins      // -12
            // 51 del      // -13
            // 53 pageup   // -16
            // 54 pagedown // -17
            // 55 home     // -14
            // 56 end      // -15
            int offset = seq[2] < 53 ? 38 : seq[2] > 54 ? 41 : 37;
            key = (Key)(0xFFFF - (seq[2] - offset));
            meta = last == '@' ? 6 : last >> 4;
        } else {
            int num = len == 5 ? ((int)seq[3] * 10 + (int)seq[4]) : (int)seq[3];
            if(num >= 25) { // ctrl + shift f1-f12 urxvt
                int offset = num == 25 || num == 26 || num == 29 ? 12 : 13;
                key = (Key)(num - offset); // TODO
                meta = Meta_CtrlShift;
            } else {
                meta = last == '@' ? 6 : last >> 4;
                key  = (Key)num; // ALSO TODO
            }
        }
    } else if(last == '~') { // 4 or 5 in length
        if(len == 5) { // shift + f1-f8, linux/urxvt
            int num = (seq[2]-48) * 10 + seq[3]-48; // f9 is 33, and should be 8
            int offset = 25;
            // TODO: mega fix this.
            meta = Meta_Shift;
            key  = (Key)(0xFFFF - (num - offset));
        } else {
            res = -1;
        }
    } else {
        res = -1;
    }
    return res;
}

int Event::parseEscSeq(char* seq, int len, const Terminal& term) {
    if(len == 1) {
        key = Key_ESC;
        return 1;
    } else if (len == 2) { // alt+char or alt+shift+char, ctrl+alt+char or alt + enter
        meta = seq[1] < 27 ? Meta_AltCtrl : \
            seq[1] >= 'A' && seq[1] <= 'Z' ? Meta_AltShift : Meta_Alt;
        switch(seq[1]) {
        case 10:
            key = Key_Enter;
            break;
        case 127:
            key = Key_Backspace;
            break;
        default:
            if (seq[1] < 27) { // ctrl+alt+char
                chr  = seq[1] + 96;
                key = (Key)seq[1];
            } else {
                chr  = seq[1];
            }
        }
        return 1;
    }
    for(int i=term.TiNKeys-1;i>=0;i--) {
        if(startsWith(seq, len, term.key(i))) {
            chr = 0;
            key = (Key)(0xFFFF-i);
            return 1;
        }
    }
    int last, num;
    switch(seq[1]) {
    case '[':
        if(parseBracketEsc(seq, len) == 0)
            return 1;
        break;
    case 'O':
        if((seq[2] > 96) && (seq[2] < 101)) { // ctrl + arrows mrxvt/urxvt
            meta = Meta_Ctrl;
            key  = (Key)(0xFFFF + (seq[2] - 118));
        } else if (seq[2] == 49) { // xfce4 arrow keys
            key  = (Key)(0xFFFF + (seq[len-1] - 80));
            meta = Meta_CtrlShift;
        }
        return 1;
        break;
    case 27: // double esc, urxvt territory
        last = seq[len-1];
      // TODO: check what happens when issuing CTRL+PAGEDOWN and then ALT+PAGEUP repeatedly
        switch(last) {
        case 'R': // mrxvt alt + f3
            meta = Meta_Alt;
            key  = (Key)(0xFFFF - 2);
            break;
        case '~': // urxvt alt + key or f1-f12
            meta = Meta_Alt;
            if(len == 6) { // f1-f12
                num = (seq[3]-48) * 10 + seq[4]-48; // f9 is 20
                key = (Key)(0xFFFF - (num - 12));
            } else { // ins/del/etc
                num = (int)seq[3];
                int offset = num < 53 ? 38 : num > 54 ? 41 : 37;
                key = (Key)(0xFFFF - (num - offset));
            }
            break;
        case '^':
        case '@': // ctrl + alt + arrows
            meta = last == '^' ? Meta_AltCtrl : Meta_AltCtrlShift;
            key  = (Key)seq[5];
          break;
        default:
            if(('a' <= last) && (last <= 'z')) { // urxvt ctr/alt arrow or ctr/shift/alt arrow
                meta = seq[2] == 91 ? Meta_AltShift : \
                    seq[4] == 'O' ? Meta_AltCtrl : Meta_AltCtrlShift;
                key  = (Key)(0xFFFF + (last - 118));
            } else if (('A' <= last) && (last <= 'Z')) { // urxvt alt + arrow keys
                meta = Meta_Alt;
                key  = (Key)(0xFFFF + (last - 86));
            } else {
                return -1;
            }
            break;
        }
        break; // case 27
    default:
        return -1;
    }
    return 1;
}

int Event::readAndExtract(int inout, const Terminal& term) {
    int nread;
    uint32_t c = 0;
    if(cutesc) {
        c = 27;
        cutesc = false;
    } else {
        while((nread = read(inout, &c, 1)) == 0);
        DEBUG("readAndExtract: nread=%d c=%d\n", nread, c);
        if(nread == -1) return -1;
    }
    type = Event_Key;
    meta = Meta_None;
    chr = 0;
    int ret;
    if(c == 27) { // escape
        ret = decodeEsc(inout, term);
    } else if(0 <= c && c <= 127) { // from ctrl-a to z, not esc
        decodeChar(c);
        ret = 1;
    } else { // utf8 sequence
        ret = decodeUtf8(inout, c);
    }
    return ret;
}

int Event::waitAndFill(struct timeval* timeout, Editor& tb) {
    const auto& term = tb.getTerm();
    int inout = tb.getFd();
    int winchFds[2] = {tb.getWinchFd(0), tb.getWinchFd(1)};
    int n;
    fd_set events;
    reset();
    if(cutesc) { // there's a part of an escape sequence left!
        n = readAndExtract(inout, term);
        if(n < 0)
            return -1;
        if(n > 0)
            return type;
    }
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
            n = readAndExtract(inout, term);
            if(n < 0)
                return -1;
            if(n > 0)
                return type;
        }
    }
}

} // end namespace teditor
