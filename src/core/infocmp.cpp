#include "infocmp.h"

namespace teditor {

InfoCmp::InfoCmp(): header(), boolCaps(), numCaps(), strCaps() {
  auto ret = check_output("infocmp -1 -G");
  ASSERT(ret.status == 0, "'infocmp' command failed! status=%d err=%s",
         ret.status, ret.error.c_str());
  auto lines = split(ret.output, '\n');
  for (const auto& line : lines) {
    // ignore comments
    if (line[0] == '#') continue;
    // header
    if (line[0] != '\t') {
      header = line;
      continue;
    }
    auto start = line.find_first_not_of('\t');
    if (start == std::string::npos) continue;
    auto eq = line.find_first_of('=');
    auto num = line.find_first_of('#');
    // boolean capability
    if (eq == std::string::npos && num == std::string::npos) {
      boolCaps.push_back(line.substr(start, line.length() - start - 1));
      continue;
    }
    // numerical capability
    if (num != std::string::npos) {
      auto name(line.substr(start, num - start));
      auto val = unsigned(str2num(line.substr(num + 1, line.length() - num - 2)));
      numCaps[name] = val;
      continue;
    }
    // string capability
    if (eq != std::string::npos) {
      auto name(line.substr(start, eq - start));
      auto val = line.substr(eq + 1, line.length() - eq - 2);
      strCaps[name] = val;
      continue;
    }
    ASSERT(false, "infocmp: line='%s' is none of bool/numerical/str capability!",
           line.c_str());
  }
}

std::string InfoCmp::getStrCap(const std::string& cap) const {
  const auto itr = strCaps.find(cap);
  ASSERT(itr != strCaps.end(), "getStrCp: unable to find capability=%s!",
         cap.c_str());
  return itr->second;
}

}  // namespace teditor
