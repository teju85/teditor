#include "testutils.h"


namespace teditor {

void setupBuff(Buffer& ml, const Pos2d<int>& pos, const Pos2d<int>& dim,
               const std::string& file, int line) {
  ml.load(file, line);
}

void setupBuffWin(Window& w, Buffers& bs, const Pos2d<int>& pos,
                  const Pos2d<int>& dim, const std::string& file, int line) {
  bs.push_back(new Buffer);
  bs.at(bs.size() - 1)->load(file, line);
  w.attachBuffs(&bs);
  w.resize(pos, dim);
}

} // end namespace teditor
