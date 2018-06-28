#pragma once

#include "../buffer.h"


namespace teditor {

class Indentor;

class CppIndentor: public Indentor {
    int indent(Buffer& buf, int line);
};

}; // end namespace teditor
