#pragma once

#include "core/mode.h"
#include "core/key_cmd_map.h"
#include "core/colors.h"


namespace teditor {


/** default mode for all readonly buffers */
class ReadOnlyMode: public Mode {
public:
    ReadOnlyMode(const std::string& n="ro",
                 const std::string& w="abcdefghijklmnopqrstuvwxyzABCDEGGHIJKLM"
                                      "NOPQRSTUVWXYZ0123456789_");
    int indent(Buffer& buf, int line) { return 0; }
    KeyCmdMap& getKeyCmdMap() { return kcMap; }
    ColorMap& getColorMap() { return cMap; }

    static Mode* create() { return new ReadOnlyMode; }

    static bool modeCheck(const std::string& file);

private:
    KeyCmdMap kcMap;
    ColorMap cMap;

    struct Keys { static std::vector<KeyCmdPair> All; };
    struct Colors { static std::vector<NameColorPair> All; };
};


}; // end namespace teditor
