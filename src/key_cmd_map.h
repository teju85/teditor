#pragma once

#include "trie.h"


namespace teditor {

enum TrieStatus {
    TS_NULL,
    TS_NON_LEAF,
    TS_LEAF
};


struct KeyCmdPair {
    std::string keySeq, cmd;
};


class KeyCmdMap {
public:
    KeyCmdMap();
    void add(const std::string& keySeq, const std::string& cmd);
    void add(const KeyCmdPair& pair) { add(pair.keySeq, pair.cmd); }
    TrieStatus traverse(const std::string& currKey);
    void resetTraversal() { currNode = nullptr; }
    const std::string getCmd() const;
    void clear();

private:
    std::unordered_map<std::string,std::string> key2cmd, cmd2key;
    Trie keyTree;
    Node* currNode;
};


struct GlobalKeys {
    static std::vector<KeyCmdPair> All;
};


struct PromptKeys {
    static std::vector<KeyCmdPair> All;
};


struct PromptYesNoKeys {
    static std::vector<KeyCmdPair> All;
};


struct DirModeKeys {
    static std::vector<KeyCmdPair> All;
};


template <typename Keys>
void populateKeyMap(KeyCmdMap& kcm, bool clear=false) {
    if(clear)
        kcm.clear();
    for(auto& kc : Keys::All)
        kcm.add(kc);
}

}; // end namespace teditor
