#pragma once

#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <string>


namespace teditor {

typedef uint16_t color_t;

static const color_t Attr_None      = 0x0000;
static const color_t Attr_Bold      = 0x0100;
static const color_t Attr_Underline = 0x0200;

struct AttrHelper {
public:
    AttrHelper();
    static color_t fromstr(const std::string& str);
    static std::string tostr(color_t attr);

private:
    std::unordered_map<std::string,color_t> amap;
    std::unordered_map<color_t,std::string> rmap;
    static const AttrHelper Str2Attr;
};


#include "def_colors.h"


struct ColorHelper {
public:
    ColorHelper();
    static color_t fromstr(const std::string& str);
    static std::string tostr(color_t colo);

private:
    std::unordered_map<std::string,color_t> amap;
    std::unordered_map<color_t,std::string> rmap;
    static const ColorHelper Str2Color;
};


struct AttrColor {
    color_t ac;

    static const color_t Mask;

    AttrColor(): ac(0) {}
    AttrColor(color_t in): ac(in) {}
    AttrColor(color_t c, color_t a): ac(a | c) {}
    void set(color_t c, color_t a) { ac = a | c; }
    void clearAttr() { ac &= Mask; }
    void setBold() { ac |= Attr_Bold; }
    void setUnderline() { ac |= Attr_Underline; }
    bool isBold() const { return ac & Attr_Bold; }
    bool isUnderline() const { return ac & Attr_Underline; }
    color_t color() const { return ac & Mask; }
    void setColor(color_t col) { ac |= (Mask & col); }
};

bool operator==(const AttrColor& a, const AttrColor& b);
bool operator!=(const AttrColor& a, const AttrColor& b);


/** name and color pair for constructing color map */
struct NameColorPair {
    std::string name, color;
};


/** Color map used for highlights/colors by the editor */
class ColorMap {
public:
    /**
     * @brief populate the map with the name-color pair
     * At first, the color string is assumed to be another key to the already
     * existing map, if that's not the case, then it is assumed to be a named
     * color string
     */
    void add(const NameColorPair& ncp);

    /** get the color for a given name */
    const AttrColor& get(const std::string& name) const;

    /** clear the contents of this map to populate it afresh */
    void clear() { colors.clear(); }

    /** convert attribute/color description into AttrColor */
    AttrColor readColor(const std::string& str);

private:
    std::unordered_map<std::string, AttrColor> colors;
};


template <typename Colors>
void populateColorMap(ColorMap& cm, bool clear=false) {
    if(clear)
        cm.clear();
    for(const auto& ncp : Colors::All)
        cm.add(ncp);
}

}; // end namespace teditor
