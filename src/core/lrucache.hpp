#pragma once

#include <unordered_map>

namespace teditor {

/**
 * @brief LRU Cache implementation
 * @tparam K key type
 * @tparam V value type
 */
template <typename K, typename V>
class LRUCache {
public:
  /**
   * @brief ctor
   * @param c capacity of the cache
   */
  LRUCache(size_t c) : objs(), cap(c), head(nullptr), tail(nullptr) {}

  bool exists(const K& k) const { return objs.find(k) != objs.end(); }
  size_t capacity() const { return cap; }
  size_t size() const { return objs.size(); }

  /**
   * @brief Gets value at the given key and if the key does not exist it returns
   *        a default value object
   */
  V& get(const K& k) {
    auto itr = objs.find(k);
    if (itr == objs.end()) {
      put(k, V());
      itr = objs.find(k);
    }
    auto& node = itr->second;
    make_recent(&node);
    return node.value;
  }

  /** puts a new element into the cache */
  void put(const K& k, const V& v) {
    if (objs.size() >= cap) remove_tail();
    auto itr = objs.find(k);
    if (itr == objs.end()) {
      Node n;
      n.key = k;
      n.prev = n.next = nullptr;
      objs[k] = n;
      itr = objs.find(k);
    }
    auto& node = itr->second;
    node.value = v;
    make_recent(&node);
  }

private:
  struct Node {
    K key;
    V value;
    Node *prev, *next;
    Node() {}
  };  // struct Node

  std::unordered_map<K, Node> objs;
  size_t cap;
  // together represent a non-allocating doubly linked list
  Node *head, *tail;

  void make_recent(Node* val) {
    if (head == val) return;
    if (tail == val) {
      tail = tail->prev;
      tail->next = nullptr;
    } else {
      if (val->prev != nullptr) val->prev->next = val->next;
      if (val->next != nullptr) val->next->prev = val->prev;
    }
    val->prev = nullptr;
    val->next = head;
    if (head != nullptr) head->prev = val;
    head = val;
    if (tail == nullptr) tail = head;
  }

  void remove_tail() {
    if (tail == nullptr) return;
    const K& key = tail->key;
    objs.erase(key);
    tail = tail->prev;
    tail->next = nullptr;
  }
};  // class LRUCache

}  // namespace teditor
