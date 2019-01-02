#include "core/pcre.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("PCRE::FindMatch") {
    Pcre p("([0-9]+)");
    auto m = p.find("Hello said 21 times!");
    REQUIRE_FALSE(m.empty());
    REQUIRE(2 == m.count());
    REQUIRE("21" == m.get(0));
    REQUIRE("21" == m.get(1));
    const auto& loc0 = m.getLoc(0);
    MatchLoc ref0 = {11, 2};
    REQUIRE(ref0 == loc0);
    REQUIRE(ref0 == m.getLoc(1));
    REQUIRE_THROWS(m.get(2));
    REQUIRE_THROWS(m.get("None"));
    REQUIRE_THROWS(m.getLoc(2));
    REQUIRE_THROWS(m.getLoc("None"));
}

TEST_CASE("PCRE::FindMatchStartPos") {
    Pcre p("\\w+");
    auto m = p.find("Hello said 21 times!", 6);
    REQUIRE_FALSE(m.empty());
    REQUIRE(1 == m.count());
    REQUIRE("said" == m.get(0));
}

TEST_CASE("PCRE::FindNoMatch") {
    Pcre p("([0-9]+)");
    auto m = p.find("Hello said none times!");
    REQUIRE(m.empty());
    REQUIRE(PCRE2_ERROR_NOMATCH == m.count());
}

TEST_CASE("PCRE::FindMultipleGroups") {
    Pcre p("([0-9]+) (\\S+)");
    auto m = p.find("Hello said 21 times!");
    REQUIRE_FALSE(m.empty());
    REQUIRE(3 == m.count());
    REQUIRE("21 times!" == m.get(0));
    REQUIRE("21" == m.get(1));
    REQUIRE("times!" == m.get(2));
}

TEST_CASE("PCRE::FindNamedGroups") {
    Pcre p("(?<Number>[0-9]+) (?<Time>\\S+)");
    auto m = p.find("Hello said 21 times!");
    REQUIRE_FALSE(m.empty());
    REQUIRE(3 == m.count());
    REQUIRE("21 times!" == m.get(0));
    REQUIRE("21" == m.get(1));
    REQUIRE("times!" == m.get(2));
    REQUIRE("times!" == m.get("Time"));
    REQUIRE("21" == m.get("Number"));
    MatchLoc ref = {14, 6};
    REQUIRE(ref == m.getLoc("Time"));
    REQUIRE_THROWS(m.get("None"));
}

TEST_CASE("Pcre::BadRegex") {
    REQUIRE_THROWS(Pcre("\\E+"));
}

TEST_CASE("PCRE::IsMatch") {
    Pcre p("([0-9]+) (\\S+)");
    REQUIRE(p.isMatch("Hello said 21 times!"));
    REQUIRE_FALSE(p.isMatch("Hello times said 21!"));
}

TEST_CASE("PCRE::Caseless") {
    Pcre p("([t]\\S+)", true);
    auto m = p.find("Hello said 21 times!");
    REQUIRE_FALSE(m.empty());
    REQUIRE(2 == m.count());
    REQUIRE("times!" == m.get(0));
    REQUIRE("times!" == m.get(1));
    REQUIRE_THROWS(m.get(2));
    REQUIRE_THROWS(m.get("None"));
    m = p.find("Hello said 21 Times!");
    REQUIRE_FALSE(m.empty());
    REQUIRE(2 == m.count());
    REQUIRE("Times!" == m.get(0));
    REQUIRE("Times!" == m.get(1));
}

TEST_CASE("PCRE::FindAll") {
    Pcre p("([0-9]+)");
    auto m = p.findAll("Hello said 21 times and 33 times!");
    REQUIRE_FALSE(m.empty());
    REQUIRE(2U == m.size());
    REQUIRE(2 == m[0].count());
    REQUIRE("21" == m[0].get(0));
    REQUIRE("21" == m[0].get(1));
    REQUIRE(2 == m[1].count());
    REQUIRE("33" == m[1].get(0));
    REQUIRE("33" == m[1].get(1));
}

TEST_CASE("PCRE::FindAllNoMatch") {
    Pcre p("([0-9]+)");
    auto m = p.findAll("Hello said none times!");
    REQUIRE(m.empty());
}

TEST_CASE("PCRE::Replace1") {
    Pcre p("Hello");
    std::string res;
    auto cnt = p.replace("Hello world!", "Beautiful", res);
    REQUIRE(1 == cnt);
    REQUIRE("Beautiful world!" == res);
}

TEST_CASE("PCRE::Replace2") {
    Pcre p("Hello");
    std::string res;
    auto cnt = p.replace("Hello Hello world!", "Beautiful", res);
    REQUIRE(1 == cnt);
    REQUIRE("Beautiful Hello world!" == res);
}

TEST_CASE("PCRE::Replace3") {
    Pcre p("Hello");
    std::string res;
    auto cnt = p.replace("No matching subs!", "Beautiful", res);
    REQUIRE(0 == cnt);
    REQUIRE("No matching subs!" == res);
}

TEST_CASE("PCRE::ReplaceAll1") {
    Pcre p("Hello");
    std::string res;
    auto cnt = p.replaceAll("Hello world!", "Beautiful", res);
    REQUIRE(1 == cnt);
    REQUIRE("Beautiful world!" == res);
}
TEST_CASE("PCRE::ReplaceAll2") {
    Pcre p("Hello");
    std::string res;
    auto cnt = p.replaceAll("Hello Hello world!", "Beautiful", res);
    REQUIRE(2 == cnt);
    REQUIRE("Beautiful Beautiful world!" == res);
}

TEST_CASE("PCRE::ReplaceAll3") {
    Pcre p("Hello");
    std::string res;
    auto cnt = p.replaceAll("No matching subs!", "Beautiful", res);
    REQUIRE(0 == cnt);
    REQUIRE("No matching subs!" == res);
}

} // end namespace teditor
