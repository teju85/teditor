#pragma once

// Thanks to: https://www.ojdip.net/2013/10/implementing-a-variant-type-in-cpp/

#include <type_traits>
#include <typeinfo>
#include "utils.h"

namespace teditor {

/**
 * @brief Data structure to help store any of the pre-defined data types.
 *        Such a thing will be useful, for eg, to support storing json objects
 */
template <typename... Types>
struct Any {
 private:
  std::type_info type;
  std::aligned_union<Types...>::type data;

  /// struct Impl ///
  template <typename... All> struct Impl;

  template <typename Head, typename... Rest>
  struct Impl<Head, Rest...> {
    static void destroy(const std::type_info& id, void* data) {
      if (id == typeid(Head)) reinterpret_cast<T*>(data)->~Head();
      else Impl<Rest..>::destroy(id, data);
    }

    static void move(const std::type_info& id, void* newData, void* oldData) {
      if (id == typeid(Head))
        new(newData) Head(std::move(*reinterpret_cast<Head*>(oldData)));
      else
        Impl<Rest...>::move(id, newData, oldData);
    }

    static void copy(const std::type_info& id, void* newData, void* oldData) {
      if (id == typeid(Head))
        new(newData) Head(*reinterpret_cast<const Head*>(oldData));
      else
        Impl<Rest...>::copy(id, newData, oldData);
    }
  };

  template <>
  struct Impl<> {
    static void destroy(const std::type_info& id, void* data) {}
    static void move(const std::type_info& id, void* newData, void* oldData) {}
    static void copy(const std::type_info& id, void* newData, void* oldData) {}
  };
  /// struct Impl ///

  std::type_info invalidType() const { return typeid(void); }

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
  bool is() const { return type == typeid(T); }

  bool valid() const { return type != invalidType(); }

  template <typename T>
  T& get() {
    ASSERT(type == typeid(T), "Any::get requested-type=%s! storage-type=%s",
           typeid(T).name(), type.name());
    return *reinterpret_cast<T*>(&data);
  }
};  // struct Any

}  // namespace teditor
