#include "isearch.h"
#include "buffer.h"
#include "logger.h"
#include "cmd_msg_bar.h"
#include "window.h"
#include <algorithm>

namespace teditor {

bool strFindEmpty(const std::string& line, const std::string& str) {
  if(str.empty()) return true;
  return strFind(line, str);
}

bool iStrFindEmpty(const std::string& line, const std::string& str) {
  if(str.empty()) return true;
  return iStrFind(line, str);
}


ISearch::ISearch(Window& w, bool _noCase):
  Choices(_noCase ? iStrFindEmpty : strFindEmpty), win(w), ml(w.getBuff()),
  curr(), matches(), noCase(_noCase) {
}

std::string ISearch::getFinalStr(size_t idx, const std::string& str) const {
  return at(idx);
}

const std::string& ISearch::at(size_t idx) const {
  return ml.at(idx).get();
}

const std::vector<size_t>& ISearch::matchesAt(size_t i) const {
  const auto itr = matches.find(i);
  ASSERT(itr != matches.end(), "No matches at line '%lu'!", i);
  return itr->second;
}

bool ISearch::updateChoices(const std::string& str) {
  if(str == curr) return false;
  reset();
  curr = str;
  if(!curr.empty()) searchBuffer();
  return true;
}

void ISearch::reset() {
  curr.clear();
  matches.clear();
}

void ISearch::searchBuffer() {
  auto len = ml.length();
  for (size_t i = 0; i < len; ++i) {
    const auto& str = ml.at(i).get();
    std::vector<size_t> res;
    noCase ? iSearchLine(str, res) : searchLine(str, res);
    if(!res.empty()) matches[i] = res;
  }
}

void ISearch::updateMainBuffer(CmdMsgBar& cmBar) {
  auto loc = cmBar.getOptLoc();
  ml.gotoLine(loc, win.dim());
}

void ISearch::searchLine(const std::string& str, std::vector<size_t>& res) {
  size_t len = str.size();
  size_t loc = 0;
  while(loc < len) {
    loc = str.find(curr, loc);
    if(loc == std::string::npos) break;
    res.push_back(loc);
    loc += curr.size();
  }
}

void ISearch::iSearchLine(const std::string& str, std::vector<size_t>& res) {
  auto itr = str.begin(), end = str.end();
  while (itr != end) {
    auto pos = std::search(itr, end, curr.begin(), curr.end(),
                           [] (char a, char b) {
                             return std::tolower(a) == std::tolower(b);
                           });
    if (pos == end) break;
    auto loc = pos - str.begin();
    res.push_back(loc);
    itr = pos + curr.size();
  }
}

} // end namespace teditor
