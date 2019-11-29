#include "core/buffer.h"
#include "catch.hpp"
#include "testutils.h"

namespace teditor {

TEST_CASE("CppMode") {
  SECTION("cpp file") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "test.cpp", 0);
    REQUIRE("c++" == ml.modeName());
  }
  SECTION("c file") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "test.c", 0);
    REQUIRE("c++" == ml.modeName());
  }
  SECTION("cxx file") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "sample.cxx", 0);
    REQUIRE("c++" == ml.modeName());
  }
  SECTION("cu file") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "test.cu", 0);
    REQUIRE("c++" == ml.modeName());
  }
  SECTION("h file") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "test.h", 0);
    REQUIRE("c++" == ml.modeName());
  }
  SECTION("hpp file") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "test.hpp", 0);
    REQUIRE("c++" == ml.modeName());
  }
  SECTION("hxx file") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "test.hxx", 0);
    REQUIRE("c++" == ml.modeName());
  }
  SECTION("cuh file") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "test.cuh", 0);
    REQUIRE("c++" == ml.modeName());
  }
  SECTION("Non C/C++ file") {
    Buffer ml;
    setupBuff(ml, {0, 0}, {30, 10}, "something-else", 0);
    REQUIRE("c++" != ml.modeName());
  }
}

TEST_CASE("CppModeIndent") {
  Buffer ml;
  setupBuff(ml, {0, 0}, {30, 10}, "samples/cpp-mode.cpp", 0);
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
