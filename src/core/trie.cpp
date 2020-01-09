#include "trie.h"
#include "utils.h"
#include <vector>


namespace teditor {

Node* Node::addNode(const std::string& key) {
  ASSERT(!leaf(), "addNode: adding node to a Leaf! key=%s", key.c_str());
  auto itr = nodes.find(key);
  if (itr == nodes.end()) {
    auto* n = new Node;
    nodes[key] = n;
    return n;
  }
  return itr->second;
}

Trie::~Trie() {
  clear();
  delete root;
}

Node* Node::getNode(const std::string& key) {
  auto itr = nodes.find(key);
  return itr == nodes.end() ? nullptr : itr->second;
}

void Node::delNode(const Strings& keys, size_t pos) {
  if (keys.empty()) return;
  auto itr = nodes.find(keys[pos]);
  if (itr == nodes.end()) return;
  if (pos == keys.size() - 1) {
    if (!itr->second->leaf()) return;
    delete itr->second;
    nodes.erase(itr->first);
    return;
  }
  // recursively try to delete the nodes
  itr->second->delNode(keys, pos + 1);
  if (itr->second->nodes.size() == 0) {
    delete itr->second;
    nodes.erase(itr->first);
  }
}

void Node::makeLeaf(const std::string& d) {
  ASSERT(nodes.empty(), "makeLeaf: Node is not empty!");
  isLeaf = true;
  leafData = d;
}

void Node::clear() {
  for (auto itr : nodes) delete itr.second;
  nodes.clear();
}


void Trie::add(const std::string& keys, const std::string& str) {
  // special case of just a space!
  if (keys == " ") {
    auto* n = root->addNode(keys);
    n->makeLeaf(str);
    return;
  }
  auto keySeq = split(keys, ' ');
  if (keySeq.size() <= 0) return;
  auto* n = root;
  for (auto k : keySeq) n = n->addNode(k);
  n->makeLeaf(str);
}

void Trie::del(const std::string& keys) {
  Strings keySeq;
  if (keys == " ") keySeq.push_back(keys);
  else keySeq = split(keys, ' ');
  root->delNode(keySeq, 0);
}

} // end namespace teditor
