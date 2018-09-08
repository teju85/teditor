#include "mode.h"
#include "buffer.h"
#include "custom/cpp_indent.h"


namespace teditor {

int Indentor::indent(Buffer& buf, int line) {
    if(!(0 < line && line < buf.length()))
        return 0;
    const auto& prev = buf.at(line-1);
    auto& curr = buf.at(line);
    int prevInd = prev.indentSize();
    int currInd = curr.indentSize();
    return std::max(0, prevInd-currInd);
}


void textMode(Mode& m) {
    populateKeyMap<GlobalKeys>(m.kcMap);
    populateColorMap<GlobalColors>(m.cMap);
    m.word = "abcdefghijklmnopqrstuvwxyzABCDEGGHIJKLMNOPQRSTUVWXYZ0123456789_";
    m.kcMap.resetTraversal();
    m.name = "text";
    m.indent = std::shared_ptr<Indentor>(new Indentor(0));
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
    m.indent = std::shared_ptr<Indentor>(new Indentor(0));
}

void cppMode(Mode& m) {
    textMode(m);
    m.name = "cpp";
    m.indent = std::shared_ptr<Indentor>(new CppIndentor(4));
}

} // end namespace teditor
