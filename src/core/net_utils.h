#pragma once

#include <string>

namespace teditor {

std::string dnsLookup(const std::string& url,
                      const std::string& domain = "domain");

}  // namespace teditor
