#pragma once

#include <stdint.h>
#include "lexer.h"

namespace teditor {
namespace parser {

template <uint32_t _id>
struct GrammarNode {
  static constexpr uint32_t TokenId = _id;
};  // struct GrammarNode


struct eps : public GrammarNode<Lexer::Empty> {
};  // struct eps


template <uint32_t _id>
struct tok : public GrammarNode<_id> {
};  // struct tok


template <uint32_t _id, typename Args...>
struct one : public GrammarNode<_id> {
};  // struct one


template <uint32_t _id, typename Gnode>
struct plus : public GrammarNode<_id> {
};  // struct plus


template <uint32_t _id, typename Gnode>
struct star : public GrammarNode<_id> {
};  // struct star


template <uint32_t _id, typename Gnode>
struct opt : public GrammarNode<_id> {
};  // struct opt


template <uint32_t _id, typename Args...>
struct seq : public GrammarNode<_id> {
};  // struct seq

}  // namespace parser
}  // namespace teditor
