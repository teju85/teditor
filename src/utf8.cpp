#include "utf8.h"

namespace teditor {
namespace Utf8 {

const unsigned char Length[] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

const unsigned char Mask[] = {0x7F, 0x1F, 0x0F, 0x07, 0x03, 0x01};

uint8_t charLen(char c) {
    return Length[(uint8_t)c];
}

int char2unicode(uint32_t* out, const char* c) {
    if(*c == 0)
        return -1;
    int len = charLen(*c);
    auto mask = Mask[len-1];
    uint32_t result = c[0] & mask;
    for(int i=1;i<len;++i) {
        result <<= 6;
        result |= c[i] & 0x3f;
    }
    *out = result;
    return len;
}

int unicode2char(char* out, uint32_t c) {
    int len = 0;
    int first;
    if(c < 0x80) {
        first = 0;
        len = 1;
    } else if(c < 0x800) {
        first = 0xc0;
        len = 2;
    } else if(c < 0x10000) {
        first = 0xe0;
        len = 3;
    } else if(c < 0x200000) {
        first = 0xf0;
        len = 4;
    } else if(c < 0x4000000) {
        first = 0xf8;
        len = 5;
    } else {
        first = 0xfc;
        len = 6;
    }
    for(int i=len-1;i>0;--i) {
        out[i] = (c & 0x3f) | 0x80;
        c >>= 6;
    }
    out[0] = c | first;
    return len;
}

} // end namespace Utf8
} // end namespace teditor
