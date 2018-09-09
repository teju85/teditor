#pragma once

#include "mode.h"
#include "key_cmd_map.h"
#include "colors.h"


namespace teditor {


/** default mode for all buffers */
class TextMode: public Mode {
public:
    TextMode();
    int indent(Buffer& buf, int line);
    KeyCmdMap& getKeyCmdMap() { return kcMap; }
    ColorMap& getColorMap() { return cMap; }

    static Mode* create() { return new TextMode; }

private:
    KeyCmdMap kcMap;
    ColorMap cMap;

    struct Keys { static std::vector<KeyCmdPair> All; };
    struct Colors { static std::vector<NameColorPair> All; };
};


}; // end namespace teditor
