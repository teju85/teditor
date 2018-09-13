#include "testutils.h"


namespace teditor {

void setupBuff(Buffer& ml, const Pos2d<int>& pos, const Pos2d<int>& dim,
               const std::string& file, int line) {
    ml.resize(pos, dim);
    ml.load(file, line);
}

} // end namespace teditor
