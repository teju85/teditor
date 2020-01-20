#pragma once

#include <stdint.h>

namespace teditor {
namespace parser {

struct GrammarNode {
};  // struct GrammarNode


template <typename uint32_t>
struct tok : public GrammarNode {
};  // struct tok


template <typename Args...>
struct one : public GrammarNode {
};  // struct one


template <typename Gnode>
struct plus : public GrammarNode {
};  // struct plus


template <typename Gnode>
struct star : public GrammarNode {
};  // struct star


template <typename Gnode>
struct opt : public GrammarNode {
};  // struct opt

}  // namespace parser
}  // namespace teditor
