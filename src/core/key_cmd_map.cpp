#include "key_cmd_map.h"
#include "utils.h"

namespace teditor {

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
  if (currNode == nullptr) currNode = keyTree.getRoot();
  currNode = currNode->getNode(currKey);
  if(currNode == nullptr) return TS_NULL;
  if(currNode->leaf()) return TS_LEAF;
  return TS_NON_LEAF;
}

const std::string KeyCmdMap::getCmd() const {
  if (currNode == nullptr || !currNode->leaf()) return "";
  return currNode->data();
}

void KeyCmdMap::eraseKey(const std::string& key) {
  auto itr = key2cmd.find(key);
  if (itr == key2cmd.end()) return;
  keyTree.del(key);
  key2cmd.erase(itr);
  for (auto& cmditr : cmd2key) {
    if (cmditr.second == key) {
      cmd2key.erase(cmditr.first);
      break;
    }
  }
}

void KeyCmdMap::clear() {
  key2cmd.clear();
  cmd2key.clear();
  keyTree.clear();
  currNode = nullptr;
}

} // end namespace teditor
