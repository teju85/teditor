#pragma once

// Thanks to: https://www.ojdip.net/2013/10/implementing-a-variant-type-in-cpp/

#include <type_traits>
#include <typeinfo>
#include "utils.h"

namespace teditor {

template <typename... Types>
struct Any {
 private:
  size_t type;
  std::aligned_union<Types...>::type data;

  /// struct Impl ///
  template <typename... All> struct Impl;

  template <typename Head, typename... Rest>
  struct Impl<Head, Rest...> {
    static void destroy(size_t id, void* data) {
      if (id == typeid(Head).hash_code()) reinterpret_cast<T*>(data)->~Head();
      else Impl<Rest..>::destroy(id, data);
    }

    static void move(size_t id, void* newData, void* oldData) {
      if (id == typeid(Head).hash_code())
        new(newData) Head(std::move(*reinterpret_cast<Head*>(oldData)));
      else
        Impl<Rest...>::move(id, newData, oldData);
    }

    static void copy(size_t id, void* newData, void* oldData) {
      if (id == typeid(Head).hash_code())
        new(newData) Head(*reinterpret_cast<const Head*>(oldData));
      else
        Impl<Rest...>::copy(id, newData, oldData);
    }
  };

  template <>
  struct Impl<> {
    static void destroy(size_t id, void* data) {}
    static void move(size_t id, void* newData, void* oldData) {}
    static void copy(size_t id, void* newData, void* oldData) {}
  };
  /// struct Impl ///

  size_t invalidType() const { return typeid(void).hash_code(); }

 public:
  Any(): type(invalidType()) {}

  Any(const Any<Types...>& old): type(old.type) {
    Impl<Types...>::copy(type, &data, &old.data);
  }

  Any(Any<Types...>&& old): type(old.type) {
    Impl<Types...>::move(type, &data, &old.data);
  }

  ~Any() { Impl<Types...>::destroy(type, &data); }

  Any<Types...>& operator=(Any<Types...>&& old) {
    data = old.data;
    old.type = invalidType();
    return *this;
  }

  Any<Types...>& operator=(Any<Types...> old) {
    std::swap(data, old.data);
    std::swap(type, old.type);
    return *this;
  }

  template <typename T>
  bool is() const { return type == typeid(T).hash_code(); }

  bool valid() const { return type != invalidType(); }

  template <typename T>
  T& get() {
    ASSERT(type == typeid(T).hash_code(),
           "Any::get invalid type requested (%s:%lu)! storage-type hash=%lu ",
           typeid(T).name(), typeid(T).hash_code(), type);
    return *reinterpret_cast<T*>(&data);
  }
};  // struct Any

}  // namespace teditor
