#include "testutils.h"


namespace teditor {

void setupBuff(Buffer& ml, const Pos2d<int>& pos, const Pos2d<int>& dim,
               const std::string& file, int line) {
  auto f = isAbs(file) ? file : rel2abs(getpwd(), file);
  ml.load(f, line);
}

void setupBuffWin(Window& w, Buffers& bs, const Pos2d<int>& pos,
                  const Pos2d<int>& dim, const std::string& file, int line) {
  bs.push_back(new Buffer);
  auto f = isAbs(file) ? file : rel2abs(getpwd(), file);
  bs.at(bs.size() - 1)->load(f, line);
  w.attachBuffs(&bs);
  w.resize(pos, dim);
}

} // end namespace teditor
