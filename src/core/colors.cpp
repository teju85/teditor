#include "colors.h"
#include "utils.h"


namespace teditor {

const AttrHelper AttrHelper::Str2Attr;

#define ADD(type) do {                          \
    amap[#type] = Attr_ ## type;                \
    rmap[Attr_ ## type] = #type;                \
  } while(0)
AttrHelper::AttrHelper(): amap(), rmap() {
  ADD(None);
  ADD(Bold);
  ADD(Underline);
  ADD(Italic);
}
#undef ADD

color_t AttrHelper::fromstr(const std::string& str) {
  const auto& itr = Str2Attr.amap.find(str);
  ASSERT(itr != Str2Attr.amap.end(),
         "AttrHelper::fromstr: Incorrect attribute '%s'!", str.c_str());
  return itr->second;
}

std::string AttrHelper::tostr(color_t attr) {
  const auto& itr = Str2Attr.rmap.find(attr);
  ASSERT(itr != Str2Attr.rmap.end(),
         "AttrHelper::tostr: Incorrect attribute id '%hu'!", attr);
  return itr->second;
}


ColorHelper& ColorHelper::get() {
  static ColorHelper obj;
  return obj;
}

color_t ColorHelper::fromstr(const std::string& str) {
  const auto& itr = get().amap.find(str);
  ASSERT(itr != get().amap.end(),
         "ColorHelper::fromstr: Incorrect color '%s'!", str.c_str());
  return itr->second;
}

std::string ColorHelper::tostr(color_t color) {
  const auto& itr = get().rmap.find(color);
  ASSERT(itr != get().rmap.end(),
         "ColorHelper::tostr: Incorrect color id '%hu'!", color);
  return itr->second;
}

bool operator==(const AttrColor& a, const AttrColor& b) { return a.ac == b.ac; }

bool operator!=(const AttrColor& a, const AttrColor& b) { return a.ac != b.ac; }

const color_t AttrColor::Mask = (color_t)(1 << 8) - 1;


void ColorMap::add(const NameColorPair& ncp) {
  const auto itr = colors.find(ncp.color);
  colors[ncp.name] = (itr!=colors.end())? itr->second : readColor(ncp.color);
}

const AttrColor& ColorMap::get(const std::string& name) const {
  const auto itr = colors.find(name);
  ASSERT(itr != colors.end(), "ColorMap: unable to find color for '%s'!",
         name.c_str());
  return itr->second;
}

AttrColor ColorMap::readColor(const std::string& str) {
  AttrColor ac;
  auto tokens = split(str, ':');
  color_t color, attr;
  if(tokens.size() == 1U) {
    color = ColorHelper::fromstr(tokens[0]);
    ac.setColor(color);
  } else if(tokens.size() == 2U) {
    attr = AttrHelper::fromstr(tokens[0]);
    color = ColorHelper::fromstr(tokens[1]);
    ac.set(color, attr);
  } else {
    ASSERT(false, "readColor: 'attr:color' is the right syntax [%s]!",
           str.c_str());
  }
  return ac;
}


} // end namespace teditor
