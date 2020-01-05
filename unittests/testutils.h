#pragma once

#include "core/utils.h"
#include "core/buffer.h"
#include "core/window.h"


namespace teditor {

void setupBuff(Buffer& ml, const Point& pos, const Point& dim,
               const std::string& file, size_t line = 0);

void setupBuffWin(Window& w, Buffers& bs, const Point& pos, const Point& dim,
                  const std::string& file, size_t line = 0);

}; // end namespace teditor
