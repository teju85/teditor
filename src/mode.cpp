#include "mode.h"
#include "buffer.h"


namespace teditor {

void textMode(Mode& m) {
    populateKeyMap<GlobalKeys>(m.kcMap);
    populateColorMap<GlobalColors>(m.cMap);
    m.word = "abcdefghijklmnopqrstuvwxyzABCDEGGHIJKLMNOPQRSTUVWXYZ0123456789_";
    m.kcMap.resetTraversal();
    m.name = "text";
    m.indent = nullptr;
}

void dirMode(Mode& m) {
    textMode(m);
    populateKeyMap<DirModeKeys>(m.kcMap);
    populateColorMap<DirColors>(m.cMap);
    m.kcMap.resetTraversal();
    m.name = "dir";
}

void cmBarMode(Mode& m) {
    populateKeyMap<PromptKeys>(m.kcMap, true);
    populateColorMap<GlobalColors>(m.cMap);
    m.word = "abcdefghijklmnopqrstuvwxyzABCDEGGHIJKLMNOPQRSTUVWXYZ0123456789_-";
    m.name = "cmbar";
    m.indent = nullptr;
}

} // end namespace teditor
