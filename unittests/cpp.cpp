#include "buffer.h"
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
    ml.resize({0, 0}, {30, 10});
    ml.load("samples/cpp-mode.cpp");
    auto& culoc = ml.getCursor().at(0);
    SECTION("post-namespace indentation") {
        culoc.y = 7;
        ml.indent();
        REQUIRE("static int thisLineNoIndent = 0;" == ml.at(7).get());
    }
    SECTION("#include and other directives") {
        culoc.y = 4;
        ml.indent();
        REQUIRE("#include <stdint.h>" == ml.at(4).get());
    }
}

} // end namespace teditor
