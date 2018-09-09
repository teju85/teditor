#include "mode.h"


namespace teditor {


ModeCreatorMap& modes() {
    static ModeCreatorMap _modes;
    return _modes;
}

Strings allModeNames() {
    auto& m = modes();
    Strings ret;
    for(const auto itr : m)
        ret.push_back(itr.first);
    return ret;
}

void Mode::registerMode(const std::string& mode, ModeCreator fptr) {
    auto& m = modes();
    ASSERT(m.find(mode) == m.end(),
           "Mode '%s' already registered!", mode.c_str());
    m[mode] = fptr;
}

ModePtr Mode::createMode(const std::string& mode) {
    auto& m = modes();
    auto itr = m.find(mode);
    ASSERT(itr != m.end(), "Mode '%s' not registered!", mode.c_str());
    ModePtr ptr(itr->second());
    return ptr;
}


} // end namespace teditor
