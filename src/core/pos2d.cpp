#include "pos2d.h"


namespace teditor {

bool Positions::operator==(const Positions& a) const {
    if(size() != a.size()) return false;
    for(size_t i=0;i<size();++i) {
        if(at(i) != a[i])
            return false;
    }
    return true;
}

void Positions::addedImpl(size_t i, const std::string& chars) {
    auto& pos = at(i);
    for(const auto& c : chars) {
        if(c == '\n') {
            moveLinesForAll(i, 1);
            moveAllOnSameLine(i, -pos.x);
        } else {
            moveAllOnSameLine(i, 1);
        }
    }
}

void Positions::added(const Strings& chars) {
    auto len = size();
    for(size_t i=0;i<len;++i) {
        addedImpl(i, chars[i]);
    }
}

void Positions::added(const std::string& chars) {
    auto len = size();
    for(size_t i=0;i<len;++i) {
        addedImpl(i, chars);
    }
}

void Positions::moveAllOnSameLine(size_t i, int delta) {
    auto& ref = at(i);
    for(size_t j=i;j<size();++j) {
        auto& loc = at(j);
        if(ref.y == loc.y)
            loc.x += delta;
    }
}

void Positions::moveLinesForAll(size_t i, int delta) {
    for(size_t j=i;j<size();++j) {
        at(j).y += delta;
    }
}


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
