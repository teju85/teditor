#include "grammar.h"

namespace teditor {
namespace parser {

Grammar::Grammar(const std::vector<TerminalDef>& terms,
                 const std::vector<NonTerminalDef>& nonterms,
                 const std::string& startSym):
  nameToId(), terminals(), nonTerminals(), tNames(), ntNames(),
  ntNameToProdIds(), start(startSym) {
  for (const auto& t : terms) addTerminal(t.first, t.second);
  for (const auto& nt : nonterms) addNonTerminal(nt.first, nt.second);
}

// all terminals appear before the non-terminals
bool Grammar::isTerminal(const std::string& sym) const {
  auto itr = nameToId.find(sym);
  if (itr == nameToId.end()) return false;
  return isTerminal(itr->second);
}

void Grammar::addTerminal(const std::string& name, const std::string& regex) {
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
  auto prodId = uint32_t(nonTerminals.size());
  if (itr == nameToId.end()) {
    id = uint32_t(terminals.size() + ntNames.size());
    nameToId[name] = id;
    ntNameToProdIds[name] = {prodId};
    ntNames.push_back(name);
  } else {
    id = itr->second;
    ntNameToProdIds[name].push_back(prodId);
  }
  nonTerminals.push_back({id, syms});
}

uint32_t Grammar::getId(const std::string& sym) const {
  const auto itr = nameToId.find(sym);
  ASSERT(itr != nameToId.end(), "getId: no symbol named '%s'!", sym.c_str());
  return itr->second;
}

std::string Grammar::getName(uint32_t id) const {
  return isTerminal(id) ? tNames[id] : ntNames[id - terminals.size()];
}

}  // namespace parser
}  // namespace teditor
