#include "grammar.h"

namespace teditor {
namespace parser {

// all terminals appear before the non-terminals
bool Grammar::isTerminal(const std::string& sym) const {
  auto itr = nameToId.find(sym);
  if (itr == nameToId.end()) return false;
  return isTerminal(itr->second);
}

void Grammar::addTerminal(const std::string& name, const std::string& regex) {
  ASSERT(nonTerminals.empty(),
         "addTerminal: cannot be called after calling addNonTerminal!");
  ASSERT(nameToId.find(name) == nameToId.end(),
         "addTerminal: Symbol with name '%s' already exists!", name.c_str());
  auto id = uint32_t(terminals.size());
  terminals.push_back(TokenDef{id, regex});
  tNames.push_back(name);
  nameToId[name] = id;
}

void Grammar::addNonTerminal(const std::string& name,
                             const std::vector<std::string>& syms) {
  auto itr = nameToId.find(name);
  uint32_t id;
  if (itr == nameToId.end()) {
    id = uint32_t(terminals.size() + ntNames.size());
    nameToId[name] = id;
    ntNames.push_back(name);
  } else {
    id = itr->second;
  }
  nonTerminals.push_back({id, syms});
}

uint32_t Grammar::getId(const std::string& name) const {
  const auto itr = nameToId.find(name);
  ASSERT(itr != nameToId.end(), "getId: no symbol named '%s'!", name.c_str());
  return itr->second;
}

std::string Grammar::getName(uint32_t id) const {
  return isTerminal(id) ? tNames[id] : ntNames[id - terminals.size()];
}

}  // namespace parser
}  // namespace teditor
