#pragma once

#include <stdint.h>

namespace teditor {
namespace Utf8 {

uint8_t charLen(char c);
int char2unicode(uint32_t* out, const char* c);
int unicode2char(char* out, uint32_t c);

}; // end namespace Utf8
}; // end namespace teditor
