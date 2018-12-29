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

} // end namespace teditor
