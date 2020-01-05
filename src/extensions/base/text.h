#pragma once

#include "readonly.h"
#include "core/key_cmd_map.h"
#include "core/colors.h"

namespace teditor {
namespace text {

/** default mode for all buffers */
class TextMode: public readonly::ReadOnlyMode {
public:
  TextMode(const std::string& n="text",
           const std::string& w="abcdefghijklmnopqrstuvwxyzABCDEGGHIJKLMNOPQRS"
                                "TUVWXYZ0123456789_");
  size_t indent(Buffer& buf, size_t line);

  static Mode* create() { return new TextMode; }
  static bool modeCheck(const std::string& file);

private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };
};

}; // end namespace text
}; // end namespace teditor
