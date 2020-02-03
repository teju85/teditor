#include "mode.h"
#include "core/option.h"

namespace teditor {
namespace watch {

WatchMode::WatchMode(): readonly::ReadOnlyMode("watch") {
  populateKeyMap<WatchMode::Keys>(getKeyCmdMap());
  populateColorMap<WatchMode::Colors>(getColorMap());
}

REGISTER_MODE(WatchMode, "watch");


std::vector<KeyCmdPair> WatchMode::Keys::All = {
  {"q", "watch-stop"},
  {"r", "watch-restart"},
};

std::vector<NameColorPair> WatchMode::Colors::All = {};

}  // end namespace watch
}  // end namespace teditor
