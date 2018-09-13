#pragma once

#include "utils.h"
#include "buffer.h"


namespace teditor {

void setupBuff(Buffer& ml, const Pos2d<int>& pos, const Pos2d<int>& dim,
               const std::string& file, int line=0);

}; // end namespace teditor
