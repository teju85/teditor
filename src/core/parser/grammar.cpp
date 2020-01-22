#include "grammar.h"

namespace teditor {
namespace parser {

const std::string Grammar::Eps("eps");
const std::string Grammar::Eof("$");

Grammar::Grammar(const std::vector<TerminalDef>& terms,
                 const std::vector<NonTerminalDef>& nonterms,
                 const std::string& startSym):
  nameToId(), terminals(), prods(), tNames(), ntNames(), ntNameToProdIds(),
  start(startSym) {
  for (const auto& t : terms) addTerminal(t.first, t.second);
  addTerminal(Eps, "");  // special erasing terminal
  addTerminal(Eof, "");  // special end-of-file terminal
  for (const auto& nt : nonterms) addNonTerminal(nt.first, nt.second);
}

std::shared_ptr<Lexer> Grammar::getLexer() const {
  std::shared_ptr<Lexer> lex(new Lexer(terminals));
  return lex;
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

void Grammar::addNonTerminal(const std::string& name, const Strings& syms) {
  auto itr = nameToId.find(name);
  uint32_t id;
  auto prodId = uint32_t(prods.size());
  if (itr == nameToId.end()) {
    id = uint32_t(terminals.size() + ntNames.size());
    nameToId[name] = id;
    ntNameToProdIds[name] = {prodId};
    ntNames.push_back(name);
  } else {
    ASSERT(!isTerminal(name), "addNonTerminal: symbol '%s' is a terminal!",
           name.c_str());
    id = itr->second;
    ntNameToProdIds[name].push_back(prodId);
  }
  prods.push_back({id, syms});
}

uint32_t Grammar::getId(const std::string& sym) const {
  const auto itr = nameToId.find(sym);
  ASSERT(itr != nameToId.end(), "getId: no symbol named '%s'!", sym.c_str());
  return itr->second;
}

std::string Grammar::getName(uint32_t id) const {
  return isTerminal(id) ? tNames[id] : ntNames[id - terminals.size()];
}

const std::vector<uint32_t>& Grammar::getProdIds(const std::string& sym) const {
  ASSERT(!isTerminal(sym), "getProdIds: symbol '%s' is a Terminal!",
         sym.c_str());
  const auto itr = ntNameToProdIds.find(sym);
  ASSERT(itr != ntNameToProdIds.end(), "getProdIds: symbol '%s' is unknown!",
         sym.c_str());
  return itr->second;
}

const std::vector<uint32_t>& Grammar::getProdIds(uint32_t id) const {
  return getProdIds(getName(id));
}

}  // namespace parser
}  // namespace teditor
