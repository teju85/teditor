#include "grammar.h"

namespace teditor {
namespace parser {

// all terminals appear before the non-terminals
bool Grammar::isTerminal(const std::string& sym) const {
  auto itr = nameToId.find(sym);
  if (itr == nameToId.end()) return false;
  if (itr->second.size() == 1) return isTerminal(itr->second[0]);
  return false;
}

void Grammar::addTerminal(const std::string& name, const std::string& regex) {
  ASSERT(nonTerminals.empty(),
         "addTerminal: cannot be called after calling addNonTerminal!");
  ASSERT(nameToId.find(name) == nameToId.end(),
         "addTerminal: Symbol with name '%s' already exists!", name.c_str());
  auto id = uint32_t(terminals.size());
  terminals.push_back(TokenDef{id, regex});
  nameToId[name] = {id};
}

void Grammar::addNonTerminal(const std::string& name,
                             const std::vector<std::string>& syms) {
  auto id = uint32_t(terminals.size() + nonTerminals.size());
  nonTerminals.push_back({id, syms});
  auto itr = nameToId.find(name);
  if (itr != nameToId.end()) itr->second.push_back(id);
  else nameToId[name] = {id};
}

void Grammar::markStart(const std::string& name) {
  auto itr = nameToId.find(name);
  ASSERT(itr != nameToId.end(), "markStart: production '%s' does not exist!",
         name.c_str());
  ASSERT(itr->second.size() == 1,
         "markStart: production '%s' does not have unique mapping! [len=%lu]",
         name.c_str(), itr->second.size());
  ASSERT(!isTerminal(name), "markStart: start rule '%s' is a terminal!",
         name.c_str());
  start = itr->second[0];
}

}  // namespace parser
}  // namespace teditor
