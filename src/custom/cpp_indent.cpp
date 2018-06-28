#include "cpp_indent.h"
#include "../mode.h"


namespace teditor {

int CppIndentor::indent(Buffer& buf, int line) {
    if(!(0 < line && line < buf.length()))
        return 0;
    const auto& prev = buf.at(line-1);
    auto& curr = buf.at(line);
    int prevInd = prev.indentSize();
    int currInd = curr.indentSize();
    return std::min(0, prevInd-currInd);
}

} // end namespace teditor
