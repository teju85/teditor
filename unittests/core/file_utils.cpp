#include "core/file_utils.h"
#include "core/utils.h"
#include <catch.hpp>

namespace teditor {

TEST_CASE("FileUtils::listDir2str") {
  auto fs = listDir("samples");
  std::string ref;
  for(const auto& f : fs) {
    auto fname = isCurrentOrParentDir(f.name)? f.name : basename(f.name);
    auto buff = format("  %10.10s  %8lu  %s\n", f.perms, f.size, fname.c_str());
    ref += buff;
  }
  REQUIRE(ref == listDir2str("samples"));
}

}  // namespace teditor
