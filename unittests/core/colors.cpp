#include "core/colors.h"
#include "catch.hpp"

namespace teditor {

TEST_CASE("Colors::AttrColor") {
  AttrColor ac;
  REQUIRE_FALSE(ac.isBold());
  REQUIRE_FALSE(ac.isUnderline());
  REQUIRE_FALSE(ac.isItalic());
  ac.setBold();
  REQUIRE(ac.isBold());
  ac.setUnderline();
  REQUIRE(ac.isUnderline());
  ac.setItalic();
  REQUIRE(ac.isItalic());
  AttrColor ac1, ac2;
  REQUIRE(ac1 == ac2);
  REQUIRE(ac != ac1);
  ac.clearAttr();
  REQUIRE(ac == ac1);
  ac.setColor(Color_Red);
  REQUIRE(Color_Red == ac.color());
}

TEST_CASE("Colors::AttrHelper") {
  REQUIRE_THROWS(AttrHelper::fromstr("Invalid"));
  REQUIRE(Attr_Bold == AttrHelper::fromstr("Bold"));
  REQUIRE("Bold" == AttrHelper::tostr(Attr_Bold));
  REQUIRE(Attr_Underline == AttrHelper::fromstr("Underline"));
  REQUIRE("Underline" == AttrHelper::tostr(Attr_Underline));
  REQUIRE(Attr_Italic == AttrHelper::fromstr("Italic"));
  REQUIRE("Italic" == AttrHelper::tostr(Attr_Italic));
}

TEST_CASE("Colors::ColorHelper") {
  REQUIRE_THROWS(ColorHelper::fromstr("Invalid"));
  REQUIRE(Color_Red == ColorHelper::fromstr("Red"));
  REQUIRE("Red" == ColorHelper::tostr(Color_Red));
}

} // end namespace teditor
