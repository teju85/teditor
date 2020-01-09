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
  KeyCmdMap(): key2cmd(), cmd2key(), keyTree(), currNode(nullptr) {}
  void add(char key, const std::string& cmd);
  void add(const std::string& keySeq, const std::string& cmd);
  void add(const KeyCmdPair& pair) { add(pair.keySeq, pair.cmd); }
  TrieStatus traverse(const std::string& currKey);
  void resetTraversal() { currNode = keyTree.getRoot(); }
  const std::string getCmd() const;
  void clear();
  void eraseKey(const std::string& key);

private:
  std::unordered_map<std::string, std::string> key2cmd, cmd2key;
  Trie keyTree;
  Node* currNode;
};


template <typename Keys>
void populateKeyMap(KeyCmdMap& kcm, bool clear=false) {
  if(clear) kcm.clear();
  for(auto& kc : Keys::All) {
    if (kc.cmd.empty()) kcm.eraseKey(kc.keySeq);
    else kcm.add(kc);
  }
}

}; // end namespace teditor
