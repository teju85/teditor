#pragma once

#include "../buffer.h"


namespace teditor {

class Indentor;

class CppIndentor: public Indentor {
public:
    CppIndentor(int i): Indentor(i) {}
    int indent(Buffer& buf, int line);
};

}; // end namespace teditor
