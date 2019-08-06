#pragma once

#include "core/utils.h"
#include "core/buffer.h"
#include "core/window.h"


namespace teditor {

void setupBuff(Buffer& ml, const Pos2d<int>& pos, const Pos2d<int>& dim,
               const std::string& file, int line=0);

void setupBuffWin(Window& w, Buffers& bs, const Pos2d<int>& pos,
                  const Pos2d<int>& dim, const std::string& file, int line=0);

}; // end namespace teditor
