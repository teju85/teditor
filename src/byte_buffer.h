#pragma once

#include <string>


namespace teditor {

class ByteBuffer: public std::string {
public:
    ByteBuffer(int cap);
    void reserve(int cap);
    void append(const char* data, int len);
    void puts(const char* data);
    void flush(int fd);
};

}; // end namespace teditor
