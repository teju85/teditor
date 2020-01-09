#pragma once

#include <unordered_map>
#include <string>
#include "keys.h"
#include "utils.h"

namespace teditor {

class Node {
public:
  Node(): nodes(), isLeaf(false), leafData() {}
  ~Node() { clear(); }
  Node* addNode(const std::string& key);
  bool delNode(const Strings& keys, size_t pos);
  Node* getNode(const std::string& key);
  void makeLeaf(const std::string& d);
  bool leaf() const { return isLeaf; }
  const std::string& data() { return leafData; }
  void clear();

private:
  std::unordered_map<std::string, Node*> nodes;
  bool isLeaf;
  std::string leafData;
};


class Trie {
public:
  Trie(): root(new Node) {}
  ~Trie();
  void add(const std::string& keys, const std::string& str);
  void del(const std::string& keys);
  Node* getNode(const std::string& key) { return root->getNode(key); }
  void clear() { root->clear(); }

private:
  Node* root;
};

}; // end namespace teditor
