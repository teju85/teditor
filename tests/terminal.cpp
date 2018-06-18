#include "terminal.h"
#include "utils.h"
#include "gtest/gtest.h"

namespace teditor {

TEST(Terminal, Sizes) {
    ASSERT_EQ(22, Terminal::TiNKeys);
    ASSERT_EQ(Func_FuncsNum-2, Terminal::TiNFuncs);
}

TEST(Terminal, Name) {
    std::string term = env("TERM");
    setenv("TERM", "xterm", 6);
    Terminal te;
    ASSERT_EQ("xterm", te.name());
    setenv("TERM", term.c_str(), term.size());
}

} // end namespace teditor
