#include "byte_buffer.h"
#include <string.h>
#include <unistd.h>
#include "logger.h"


namespace teditor {

ByteBuffer::ByteBuffer(int cap): std::string() {
    reserve(cap);
}

void ByteBuffer::reserve(int cap) {
    int bcap = capacity();
    if(bcap >= cap)
        return;
    // prefer doubling capacity
    if(bcap*2 > cap)
        cap = bcap * 2;
    std::string::reserve(cap);
}

void ByteBuffer::append(const char* data, int len) {
    reserve(length()+len);
    std::string::append(data, len);
}

void ByteBuffer::puts(const char* data) {
    append(data, strlen(data));
}

void ByteBuffer::flush(int fd) {
    write(fd, c_str(), length());
    ULTRA_DEBUG("flush: len=%d buff=%s\n", length(), c_str());
    clear();
}

} // end namespace teditor
