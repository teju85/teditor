#include "mode.h"


namespace teditor {

ModeCreatorMap& modes() {
  static ModeCreatorMap _modes;
  return _modes;
}

ModeInferMap& infers() {
  static ModeInferMap _infers;
  return _infers;
}

Strings allModeNames() {
  auto& m = modes();
  Strings ret;
  // 'cmbar' is not really a mode per-se!
  for(const auto itr : m)
    if(itr.first != "cmbar") ret.push_back(itr.first);
  return ret;
}

bool baseModeCmdFilterOp(const std::string& name) {
  return !(name[0] == '.' || name.find("::") != std::string::npos);
}

Strings Mode::cmdNames() const {
  return allCmdNames(baseModeCmdFilterOp);
}

Mode::Registrar::Registrar(const std::string& mode, ModeCreator fptr,
                           InferMode iptr) {
  auto& m = modes();
  ASSERT(m.find(mode) == m.end(), "Mode '%s' is already registered!",
         mode.c_str());
  m[mode] = fptr;
  infers()[mode] = iptr;
}

std::string Mode::inferMode(const std::string& file, bool isDir) {
  auto& i = infers();
  if(isDir) return "dir";  // special case for directories
  for(const auto itr : i)
    if(itr.second(file)) return itr.first;
  return "text";  // default mode
}

ModePtr Mode::createMode(const std::string& mode) {
  auto& m = modes();
  auto itr = m.find(mode);
  ASSERT(itr != m.end(), "Mode '%s' not registered!", mode.c_str());
  ModePtr ptr(itr->second());
  return ptr;
}

} // end namespace teditor
