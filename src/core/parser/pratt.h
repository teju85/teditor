#pragma once

#include "lexer.h"
#include "parse_tree.h"

namespace teditor {
namespace parser {

Node* expression(Token& token, Lexer& lex, int rbp = 0);

}  // namespace parser
}  // namespace teditor
