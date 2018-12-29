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

void Positions::update(const Strings& chars) {
    for(size_t i=0;i<size();++i) {
        at(i) += chars[i];
    }
}

} // end namespace teditor
