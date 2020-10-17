#include "core/file_utils.h"
#include "core/utils.h"
#include <catch.hpp>

namespace teditor {

TEST_CASE("FilesHist") {
    const std::string file("test_hist.txt");
    remove(file.c_str());

    FilesHist fh(file, 3);
    REQUIRE_FALSE(isFile(file.c_str()));
    REQUIRE(fh.empty());
    auto vec = fh.get();
    REQUIRE(vec.empty());

    // adding fresh one
    fh.add("main.cpp", 0);
    REQUIRE(1U == fh.size());
    REQUIRE(1U == fh.size());
    vec = fh.get();
    REQUIRE_FALSE(vec.empty());
    REQUIRE("main.cpp;0" == vec[0]);

    // adding duplicate
    fh.add("main.cpp", 1);
    vec = fh.get();
    REQUIRE(1U == fh.size());
    REQUIRE("main.cpp;1" == vec[0]);

    fh.add("tests", 0);
    vec = fh.get();
    REQUIRE(2U == fh.size());
    REQUIRE("tests;0" == vec[0]);

    fh.add("src", 0);
    vec = fh.get();
    REQUIRE(3U == fh.size());
    REQUIRE("src;0" == vec[0]);

    // crossing the limit
    fh.add("Doxyfile", 10);
    vec = fh.get();
    REQUIRE(3U == fh.size());
    REQUIRE("Doxyfile;10" == vec[0]);
    REQUIRE("src;0" == vec[1]);
    REQUIRE("tests;0" == vec[2]);

    // storage and reload it
    fh.store();
    REQUIRE(isFile(file.c_str()));
    FilesHist fh1(file, 2);
    vec = fh1.get();
    REQUIRE(2U == fh1.size());
    REQUIRE("Doxyfile;10" == vec[0]);
    REQUIRE("src;0" == vec[1]);

    remove(file.c_str());
}

}  // namespace teditor
