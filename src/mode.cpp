#include "mode.h"


namespace teditor {

Mode textMode() {
    Mode m;
    populateKeyMap<GlobalKeys>(m.kcMap);
    populateColorMap<GlobalColors>(cMap);
    m.word = "abcdefghijklmnopqrstuvwxyzABCDEGGHIJKLMNOPQRSTUVWXYZ0123456789_";
    m.kcMap.resetTraversal();
    return m;
}

Mode dirMode() {
    auto m = textMode();
    populateKeyMap<DirModeKeys>(m.kcMap);
    m.kcMap.resetTraversal();
    return m;
}

} // end namespace teditor
