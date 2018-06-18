#include "colors.h"
#include "utils.h"


namespace teditor {

const AttrHelper AttrHelper::Str2Attr;

#define ADD(type) do {                           \
        amap[#type] = Attr_ ## type;             \
        rmap[Attr_ ## type] = #type;             \
    } while(0)
AttrHelper::AttrHelper(): amap(), rmap() {
    ADD(None);
    ADD(Bold);
    ADD(Underline);
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

const ColorHelper ColorHelper::Str2Color;

#define ADD(type) do {                          \
        amap[#type] = Color_ ## type;           \
        rmap[Color_ ## type] = #type;           \
    } while(0)
ColorHelper::ColorHelper(): amap(), rmap() {
    ADD(Black);
    ADD(Maroon);
    ADD(Green);
    ADD(Olive);
    ADD(Navy);
    ADD(Purple);
    ADD(Teal);
    ADD(Silver);
    ADD(Grey);
    ADD(Red);
    ADD(Lime);
    ADD(Yellow);
    ADD(Blue);
    ADD(Fuchsia);
    ADD(Aqua);
    ADD(White);
    ADD(Grey0);
    ADD(NavyBlue);
    ADD(DarkBlue);
    ADD(Blue3);
    //ADD(Blue3);
    ADD(Blue1);
    ADD(DarkGreen);
    ADD(DeepSkyBlue4);
    //ADD(DeepSkyBlue4);
    //ADD(DeepSkyBlue4);
    ADD(DodgerBlue3);
    ADD(DodgerBlue2);
    ADD(Green4);
    ADD(SpringGreen4);
    ADD(Turquoise4);
    ADD(DeepSkyBlue3);
    //ADD(DeepSkyBlue3);
    ADD(DodgerBlue1);
    ADD(Green3);
    ADD(SpringGreen3);
    ADD(DarkCyan);
    ADD(LightSeaGreen);
    ADD(DeepSkyBlue2);
    ADD(DeepSkyBlue1);
    //ADD(Green3);
    //ADD(SpringGreen3);
    ADD(SpringGreen2);
    ADD(Cyan3);
    ADD(DarkTurquoise);
    ADD(Turquoise2);
    ADD(Green1);
    //ADD(SpringGreen2);
    ADD(SpringGreen1);
    ADD(MediumSpringGreen);
    ADD(Cyan2);
    ADD(Cyan1);
    ADD(DarkRed);
    ADD(DeepPink4);
    ADD(Purple4);
    //ADD(Purple4);
    ADD(Purple3);
    ADD(BlueViolet);
    ADD(Orange4);
    ADD(Grey37);
    ADD(MediumPurple4);
    ADD(SlateBlue3);
    //ADD(SlateBlue3);
    ADD(RoyalBlue1);
    ADD(Chartreuse4);
    ADD(DarkSeaGreen4);
    ADD(PaleTurquoise4);
    ADD(SteelBlue);
    ADD(SteelBlue3);
    ADD(CornflowerBlue);
    ADD(Chartreuse3);
    //ADD(DarkSeaGreen4);
    ADD(CadetBlue);
    //ADD(CadetBlue);
    ADD(SkyBlue3);
    //ADD(SteelBlue1);
    //ADD(Chartreuse3);
    ADD(PaleGreen3);
    ADD(SeaGreen3);
    ADD(Aquamarine3);
    ADD(MediumTurquoise);
    ADD(SteelBlue1);
    ADD(Chartreuse2);
    ADD(SeaGreen2);
    ADD(SeaGreen1);
    //ADD(SeaGreen1);
    ADD(Aquamarine1);
    ADD(DarkSlateGray2);
    //ADD(DarkRed);
    //ADD(DeepPink4);
    ADD(DarkMagenta);
    //ADD(DarkMagenta);
    ADD(DarkViolet);
    //ADD(Purple);
    //ADD(Orange4);
    ADD(LightPink4);
    ADD(Plum4);
    ADD(MediumPurple3);
    //ADD(MediumPurple3);
    ADD(SlateBlue1);
    ADD(Yellow4);
    ADD(Wheat4);
    ADD(Grey53);
    ADD(LightSlateGrey);
    ADD(MediumPurple);
    ADD(LightSlateBlue);
    //ADD(Yellow4);
    ADD(DarkOliveGreen3);
    ADD(DarkSeaGreen);
    ADD(LightSkyBlue3);
    //ADD(LightSkyBlue3);
    ADD(SkyBlue2);
    //ADD(Chartreuse2);
    //ADD(DarkOliveGreen3);
    //ADD(PaleGreen3);
    ADD(DarkSeaGreen3);
    ADD(DarkSlateGray3);
    ADD(SkyBlue1);
    ADD(Chartreuse1);
    ADD(LightGreen);
    //ADD(LightGreen);
    ADD(PaleGreen1);
    //ADD(Aquamarine1);
    ADD(DarkSlateGray1);
    ADD(Red3);
    //ADD(DeepPink4);
    ADD(MediumVioletRed);
    ADD(Magenta3);
    //ADD(DarkViolet);
    //ADD(Purple);
    ADD(DarkOrange3);
    ADD(IndianRed);
    ADD(HotPink3);
    ADD(MediumOrchid3);
    ADD(MediumOrchid);
    ADD(MediumPurple2);
    ADD(DarkGoldenrod);
    ADD(LightSalmon3);
    ADD(RosyBrown);
    ADD(Grey63);
    //ADD(MediumPurple2);
    ADD(MediumPurple1);
    ADD(Gold3);
    ADD(DarkKhaki);
    ADD(NavajoWhite3);
    ADD(Grey69);
    ADD(LightSteelBlue3);
    ADD(LightSteelBlue);
    ADD(Yellow3);
    //ADD(DarkOliveGreen3);
    //ADD(DarkSeaGreen3);
    ADD(DarkSeaGreen2);
    ADD(LightCyan3);
    ADD(LightSkyBlue1);
    ADD(GreenYellow);
    ADD(DarkOliveGreen2);
    //ADD(PaleGreen1);
    //ADD(DarkSeaGreen2);
    ADD(DarkSeaGreen1);
    ADD(PaleTurquoise1);
    //ADD(Red3);
    ADD(DeepPink3);
    //ADD(DeepPink3);
    //ADD(Magenta3);
    //ADD(Magenta3);
    ADD(Magenta2);
    //ADD(DarkOrange3);
    //ADD(IndianRed);
    //ADD(HotPink3);
    ADD(HotPink2);
    ADD(Orchid);
    ADD(MediumOrchid1);
    ADD(Orange3);
    //ADD(LightSalmon3);
    ADD(LightPink3);
    ADD(Pink3);
    ADD(Plum3);
    ADD(Violet);
    //ADD(Gold3);
    ADD(LightGoldenrod3);
    ADD(Tan);
    ADD(MistyRose3);
    ADD(Thistle3);
    ADD(Plum2);
    //ADD(Yellow3);
    ADD(Khaki3);
    ADD(LightGoldenrod2);
    ADD(LightYellow3);
    ADD(Grey84);
    ADD(LightSteelBlue1);
    ADD(Yellow2);
    ADD(DarkOliveGreen1);
    //ADD(DarkOliveGreen1);
    //ADD(DarkSeaGreen1);
    ADD(Honeydew2);
    ADD(LightCyan1);
    ADD(Red1);
    ADD(DeepPink2);
    ADD(DeepPink1);
    //ADD(DeepPink1);
    //ADD(Magenta2);
    ADD(Magenta1);
    ADD(OrangeRed1);
    ADD(IndianRed1);
    //ADD(IndianRed1);
    ADD(HotPink);
    //ADD(HotPink);
    //ADD(MediumOrchid1);
    ADD(DarkOrange);
    ADD(Salmon1);
    ADD(LightCoral);
    ADD(PaleVioletRed1);
    ADD(Orchid2);
    ADD(Orchid1);
    ADD(Orange1);
    ADD(SandyBrown);
    ADD(LightSalmon1);
    ADD(LightPink1);
    ADD(Pink1);
    ADD(Plum1);
    ADD(Gold1);
    //ADD(LightGoldenrod2);
    //ADD(LightGoldenrod2);
    ADD(NavajoWhite1);
    ADD(MistyRose1);
    ADD(Thistle1);
    ADD(Yellow1);
    ADD(LightGoldenrod1);
    ADD(Khaki1);
    ADD(Wheat1);
    ADD(Cornsilk1);
    ADD(Grey100);
    ADD(Grey3);
    ADD(Grey7);
    ADD(Grey11);
    ADD(Grey15);
    ADD(Grey19);
    ADD(Grey23);
    ADD(Grey27);
    ADD(Grey30);
    ADD(Grey35);
    ADD(Grey39);
    ADD(Grey42);
    ADD(Grey46);
    ADD(Grey50);
    ADD(Grey54);
    ADD(Grey58);
    ADD(Grey62);
    ADD(Grey66);
    ADD(Grey70);
    ADD(Grey74);
    ADD(Grey78);
    ADD(Grey82);
    ADD(Grey85);
    ADD(Grey89);
    ADD(Grey93);

    ADD(DarkestGray);
    ADD(DarkerGray);
    ADD(DarkGray);
    ADD(LightGray);
    ADD(LightestGray);
}
#undef ADD

color_t ColorHelper::fromstr(const std::string& str) {
    const auto& itr = Str2Color.amap.find(str);
    ASSERT(itr != Str2Color.amap.end(),
           "ColorHelper::fromstr: Incorrect color '%s'!", str.c_str());
    return itr->second;
}

std::string ColorHelper::tostr(color_t color) {
    const auto& itr = Str2Color.rmap.find(color);
    ASSERT(itr != Str2Color.rmap.end(),
           "ColorHelper::tostr: Incorrect color id '%hu'!", color);
    return itr->second;
}

bool operator==(const AttrColor& a, const AttrColor& b) {
    return a.ac == b.ac;
}

bool operator!=(const AttrColor& a, const AttrColor& b) {
    return a.ac != b.ac;
}

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


#include "custom/global_color_map.h"

} // end namespace teditor
