#pragma once

#include <unordered_map>
#include <string>
#include "keys.h"


namespace teditor {

class Node {
public:
  Node(): nodes(), isLeaf(false), leafData() {}
  ~Node() { clear(); }
  Node* addNode(const std::string& key);
  Node* getNode(const std::string& key);
  void makeLeaf(const std::string& d);
  bool leaf() const { return isLeaf; }
  const std::string& data() { return leafData; }
  void clear();

private:
  std::unordered_map<std::string,Node*> nodes;
  bool isLeaf;
  std::string leafData;
};


class Trie {
public:
  Trie(): root() {}
  void add(const std::string& keys, const std::string& str);
  Node* getNode(const std::string& key) { return root.getNode(key); }
  void clear() { root.clear(); }

private:
  Node root;
};

}; // end namespace teditor
