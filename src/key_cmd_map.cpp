#include "key_cmd_map.h"
#include "utils.h"


namespace teditor {

KeyCmdMap::KeyCmdMap(): key2cmd(), cmd2key(), keyTree(), currNode(nullptr) {
}

void KeyCmdMap::add(char key, const std::string& cmd) {
    std::string str;
    str += key;
    add(str, cmd);
}

void KeyCmdMap::add(const std::string& keySeq, const std::string& cmd) {
    keyTree.add(keySeq, cmd);
    key2cmd[keySeq] = cmd;
    cmd2key[cmd] = keySeq;
}

TrieStatus KeyCmdMap::traverse(const std::string& currKey) {
    currNode = (currNode == nullptr)?    \
        keyTree.getNode(currKey) :       \
        currNode->getNode(currKey);
    if(currNode == nullptr)
        return TS_NULL;
    if(currNode->leaf())
        return TS_LEAF;
    return TS_NON_LEAF;
}

const std::string KeyCmdMap::getCmd() const {
    if(currNode == nullptr)
        return "";
    return currNode->data();
}

void KeyCmdMap::clear() {
    key2cmd.clear();
    cmd2key.clear();
    keyTree.clear();
    currNode = nullptr;
}


#include "custom/prompt_yesno_key_map.h"

} // end namespace teditor
