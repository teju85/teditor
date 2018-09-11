#include "keys.h"
#include "catch.hpp"


namespace teditor {

TEST_CASE("Keys::MetaKey") {
    MetaKey mk;
    REQUIRE(0xFFFF0000U == mk.getMeta());
    REQUIRE(0xFFFFU == mk.getKey());
    REQUIRE(mk.isInvalid());
    REQUIRE("" == mk.toStr());

    MetaKey mk1(Meta_Ctrl | Key_a);
    REQUIRE(Key_a == mk1.getKey());
    REQUIRE(Meta_Ctrl == mk1.getMeta());
    REQUIRE_FALSE(mk1.isInvalid());
    REQUIRE_FALSE(mk1.isAscii());
    REQUIRE("C-a" == mk1.toStr());

    MetaKey mk2(Meta_None | Key_a);
    REQUIRE(Key_a == mk2.getKey());
    REQUIRE(Meta_None == mk2.getMeta());
    REQUIRE_FALSE(mk2.isInvalid());
    REQUIRE(mk2.isAscii());
    REQUIRE("a" == mk2.toStr());

    MetaKey mk3(Meta_None, Key_a);
    REQUIRE(Key_a == mk3.getKey());
    REQUIRE(Meta_None == mk3.getMeta());
    REQUIRE_FALSE(mk3.isInvalid());
    REQUIRE(mk3.isAscii());
    REQUIRE("a" == mk3.toStr());
    mk3.reset();
    REQUIRE("" == mk3.toStr());
    REQUIRE(mk3.isInvalid());
    mk3.setKey(Key_B);
    mk3.setMeta(Meta_None);
    REQUIRE(Key_B == mk3.getKey());
    REQUIRE("B" == mk3.toStr());
    mk3.setMeta(Meta_None);
    REQUIRE(Meta_None == mk3.getMeta());
    REQUIRE(mk3.isAscii());
    REQUIRE("B" == mk3.toStr());
    mk3.updateMeta(Meta_Alt);
    REQUIRE_FALSE(mk3.isAscii());
    REQUIRE("M-B" == mk3.toStr());

    MetaKey mk4(Meta_None, Key_F1);
    REQUIRE(Key_F1 == mk4.getKey());
    REQUIRE(Meta_None == mk4.getMeta());
    REQUIRE_FALSE(mk4.isInvalid());
    REQUIRE_FALSE(mk4.isAscii());
    REQUIRE("F1" == mk4.toStr());
}

} // end namespace teditor
