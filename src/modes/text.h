#pragma once

#include "core/mode.h"
#include "core/key_cmd_map.h"
#include "core/colors.h"


namespace teditor {


/** default mode for all buffers */
class TextMode: public Mode {
public:
    TextMode();
    int indent(Buffer& buf, int line);
    KeyCmdMap& getKeyCmdMap() { return kcMap; }
    ColorMap& getColorMap() { return cMap; }

    static Mode* create() { return new TextMode; }

    static bool modeCheck(const std::string& file);

private:
    KeyCmdMap kcMap;
    ColorMap cMap;

    struct Keys { static std::vector<KeyCmdPair> All; };
    struct Colors { static std::vector<NameColorPair> All; };
};


}; // end namespace teditor
