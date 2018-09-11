#include "terminal.h"
#include "utils.h"
#include "catch.hpp"

namespace teditor {

TEST_CASE("Terminal::Sizes") {
    REQUIRE(22 == Terminal::TiNKeys);
    REQUIRE(Func_FuncsNum-2 == Terminal::TiNFuncs);
}

TEST_CASE("Terminal::Name") {
    std::string term = env("TERM");
    setenv("TERM", "xterm", 6);
    Terminal te;
    REQUIRE("xterm" == te.name());
    setenv("TERM", term.c_str(), term.size());
}

} // end namespace teditor
