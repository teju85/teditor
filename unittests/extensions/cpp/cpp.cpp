#include "core/buffer.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("CppMode") {
    SECTION("cpp file") {
        Buffer ml;
        ml.load("test.cpp", 0);
        REQUIRE("c++" == ml.modeName());
    }
    SECTION("c file") {
        Buffer ml;
        ml.load("test.c", 0);
        REQUIRE("c++" == ml.modeName());
    }
    SECTION("cxx file") {
        Buffer ml;
        ml.load("samples/sample.cxx", 0);
        REQUIRE("c++" == ml.modeName());
    }
    SECTION("cu file") {
        Buffer ml;
        ml.load("test.cu", 0);
        REQUIRE("c++" == ml.modeName());
    }
    SECTION("h file") {
        Buffer ml;
        ml.load("test.h", 0);
        REQUIRE("c++" == ml.modeName());
    }
    SECTION("hpp file") {
        Buffer ml;
        ml.load("test.hpp", 0);
        REQUIRE("c++" == ml.modeName());
    }
    SECTION("cuh file") {
        Buffer ml;
        ml.load("test.cuh", 0);
        REQUIRE("c++" == ml.modeName());
    }
    SECTION("Non C/C++ file") {
        Buffer ml;
        ml.load("something-else", 0);
        REQUIRE("c++" != ml.modeName());
    }
}

TEST_CASE("CppModeIndent") {
  Buffer ml;
  ml.load("samples/cpp-mode.cpp");
  SECTION("post-namespace indentation") {
    for(int i=0;i<7;++i) ml.down();
    ml.indent();
    REQUIRE("static int thisLineNoIndent = 0;" == ml.at(7).get());
  }
  SECTION("#include and other directives") {
    for(int i=0;i<4;++i) ml.down();
    ml.indent();
    REQUIRE("#include <stdint.h>" == ml.at(4).get());
  }
}

} // end namespace teditor