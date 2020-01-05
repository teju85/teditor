#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "cmd_msg_bar.h"
#include "logger.h"


namespace teditor {

class Buffer;
class CmdMsgBar;
class Window;

/**
 * @brief Incremental search support used by Ctrl-F command
 */
class ISearch: public Choices {
public:
  ISearch(Window& w, bool _noCase);

  const std::string& at(size_t idx) const;
  size_t size() const { return ml.length(); }
  bool updateChoices(const std::string& str);
  std::string getFinalStr(size_t idx, const std::string& str) const;
  void updateMainBuffer(CmdMsgBar& cmBar);

  /** reset the search state */
  void reset();

  /** check whether there are matches at a given line num */
  bool emptyAt(size_t i) const { return matches.find(i) == matches.end(); }

  /** fetch matches for a given line num */
  const std::vector<size_t>& matchesAt(size_t i) const;

private:
  /** current window which has this buffer */
  Window& win;
  /** buffer where to conduct searches */
  Buffer& ml;
  /** current search string */
  std::string curr;
  /** line number v/s list of matches */
  std::unordered_map<size_t, std::vector<size_t>> matches;
  /** case insensitive search? */
  bool noCase;

  void searchBuffer();
  void searchLine(const std::string& str, std::vector<size_t>& res);
  void iSearchLine(const std::string& str, std::vector<size_t>& res);
};

} // end namespace teditor
