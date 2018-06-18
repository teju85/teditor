#include "gtest/gtest.h"
#include "pcre.h"


namespace teditor {

TEST(PCRE, FindMatch) {
    Pcre p("([0-9]+)");
    auto m = p.find("Hello said 21 times!");
    ASSERT_FALSE(m.empty());
    ASSERT_EQ(2, m.count());
    ASSERT_EQ("21", m.get(0));
    ASSERT_EQ("21", m.get(1));
    const auto& loc0 = m.getLoc(0);
    MatchLoc ref0 = {11, 2};
    ASSERT_EQ(ref0, loc0);
    ASSERT_EQ(ref0, m.getLoc(1));
    ASSERT_THROW(m.get(2), std::runtime_error);
    ASSERT_THROW(m.get("None"), std::runtime_error);
    ASSERT_THROW(m.getLoc(2), std::runtime_error);
    ASSERT_THROW(m.getLoc("None"), std::runtime_error);
}

TEST(PCRE, FindMatchStartPos) {
    Pcre p("\\w+");
    auto m = p.find("Hello said 21 times!", 6);
    ASSERT_FALSE(m.empty());
    ASSERT_EQ(1, m.count());
    ASSERT_EQ("said", m.get(0));
}

TEST(PCRE, FindNoMatch) {
    Pcre p("([0-9]+)");
    auto m = p.find("Hello said none times!");
    ASSERT_TRUE(m.empty());
    ASSERT_EQ(PCRE2_ERROR_NOMATCH, m.count());
}

TEST(PCRE, FindMultipleGroups) {
    Pcre p("([0-9]+) (\\S+)");
    auto m = p.find("Hello said 21 times!");
    ASSERT_FALSE(m.empty());
    ASSERT_EQ(3, m.count());
    ASSERT_EQ("21 times!", m.get(0));
    ASSERT_EQ("21", m.get(1));
    ASSERT_EQ("times!", m.get(2));
}

TEST(PCRE, FindNamedGroups) {
    Pcre p("(?<Number>[0-9]+) (?<Time>\\S+)");
    auto m = p.find("Hello said 21 times!");
    ASSERT_FALSE(m.empty());
    ASSERT_EQ(3, m.count());
    ASSERT_EQ("21 times!", m.get(0));
    ASSERT_EQ("21", m.get(1));
    ASSERT_EQ("times!", m.get(2));
    ASSERT_EQ("times!", m.get("Time"));
    ASSERT_EQ("21", m.get("Number"));
    MatchLoc ref = {14, 6};
    ASSERT_EQ(ref, m.getLoc("Time"));
    ASSERT_THROW(m.get("None"), std::runtime_error);
}

TEST(Pcre, BadRegex) {
    ASSERT_THROW(Pcre("\\E+"), std::runtime_error);
}

TEST(PCRE, IsMatch) {
    Pcre p("([0-9]+) (\\S+)");
    ASSERT_TRUE(p.isMatch("Hello said 21 times!"));
    ASSERT_FALSE(p.isMatch("Hello times said 21!"));
}

TEST(PCRE, Caseless) {
    Pcre p("([t]\\S+)", true);
    auto m = p.find("Hello said 21 times!");
    ASSERT_FALSE(m.empty());
    ASSERT_EQ(2, m.count());
    ASSERT_EQ("times!", m.get(0));
    ASSERT_EQ("times!", m.get(1));
    ASSERT_THROW(m.get(2), std::runtime_error);
    ASSERT_THROW(m.get("None"), std::runtime_error);
    m = p.find("Hello said 21 Times!");
    ASSERT_FALSE(m.empty());
    ASSERT_EQ(2, m.count());
    ASSERT_EQ("Times!", m.get(0));
    ASSERT_EQ("Times!", m.get(1));
}

TEST(PCRE, FindAll) {
    Pcre p("([0-9]+)");
    auto m = p.findAll("Hello said 21 times and 33 times!");
    ASSERT_FALSE(m.empty());
    ASSERT_EQ(2U, m.size());
    ASSERT_EQ(2, m[0].count());
    ASSERT_EQ("21", m[0].get(0));
    ASSERT_EQ("21", m[0].get(1));
    ASSERT_EQ(2, m[1].count());
    ASSERT_EQ("33", m[1].get(0));
    ASSERT_EQ("33", m[1].get(1));
}

TEST(PCRE, FindAllNoMatch) {
    Pcre p("([0-9]+)");
    auto m = p.findAll("Hello said none times!");
    ASSERT_TRUE(m.empty());
}

} // end namespace teditor
