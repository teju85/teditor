#include "cursor.h"


namespace teditor {

void Regions::enable(const Positions& p) {
    clear();
    resize(p.size());
    std::copy(p.begin(), p.end(), begin());
}

bool Regions::isInside(int y, int x, const Positions& cu, int i) const {
    const auto& p = at(i);
    const auto& culoc = cu[i];
    Pos2d<int> start, end;
    p.find(start, end, culoc);
    if(start.y < y && y < end.y) return true;
    else if(start.y == end.y && start.y == y) {
        if(start.x <= x && x <= end.x) return true;
    } else if(start.y == y && x >= start.x) return true;
    else if(end.y == y && x <= end.x) return true;
    return false;
}

bool Regions::isInside(int y, int x, const Positions& cu) const {
    if(empty()) return false;
    int len = (int)cu.size();
    for(int i=0;i<len;++i)
        if(isInside(y, x, cu, i)) return true;
    return false;
}

} // end namespace teditor
