#pragma once

#include "utils.h"
#include <unordered_map>

namespace teditor {

struct InfoCmp {
  // header
  std::string header;
  // boolean capabilities
  Strings boolCaps;
  // numerical capabilities
  std::unordered_map<std::string, unsigned> numCaps;
  // string capabilities
  std::unordered_map<std::string, std::string> strCaps;

  InfoCmp();

  std::string getStrCap(const std::string& cap) const;
};  // struct InfoCmp

}  // namespace teditor
