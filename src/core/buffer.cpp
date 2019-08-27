#include "buffer.h"
#include <cstring>
#include <unistd.h>
#include "editor.h"
#include "logger.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "window.h"


namespace teditor {

Buffer::Buffer(const std::string& name, bool noUndoRedo):
  lines(), startLine(0), modified(false), readOnly(false), buffName(name),
  fileName(), dirName(), regions(), mode(Mode::createMode("text")), locs(),
  undoStack(), redoStack(), disableStack(noUndoRedo) {
  addLine();
  dirName = getpwd();
  locs.push_back(Pos2di(0, 0));
  begin();
}

////// Start: Buffer editing //////
void Buffer::insert(char c) {
  OpData op;
  op.type = OpInsert;
  op.strs = Strings(locs.size(), std::string(1, c));
  applyInsertOp(op);
}

void Buffer::insert(const Strings& strs) {
  OpData op;
  op.type = OpInsert;
  op.strs = strs;
  applyInsertOp(op);
}

void Buffer::insert(const std::string& buf) {
  OpData op;
  op.type = OpInsert;
  op.strs = Strings(locs.size(), buf);
  applyInsertOp(op);
}

///@todo: what if a single line crosses the whole screen!?
void Buffer::insert(char c, size_t i) {
  auto& pos = locs[i];
  if(c == '\n' || c == (char)Key_Enter) {
    auto newLine = at(pos.y).split(pos.x);
    lines.insert(lines.begin() + pos.y + 1, newLine);
    // inserting line at the current cursor means all other cursors below it
    // must also be moved down by one line to preserve their right location
    moveDownAllNextCursors(i);
    pos.x = 0;
    return;
  }
  auto& line = lines[pos.y];
  line.insert(c, pos.x);
  // inserting a char should move other cursors in the same line!
  moveRightCursorsOnSameLine(i);
}

Strings Buffer::removeAndCopy() {
  OpData op;
  op.type = OpDelete;
  if(!isRegionActive()) return op.strs;
  ///@todo: what if 'before' is after 'after'!?
  op.before = copyCursors(regions);
  op.after = saveCursors();
  op.strs = removeRegion(op.before, op.after);
  restoreCursors(op.before);
  stopRegion();
  lineDown();
  pushNewOp(op);
  modified = true;
  return op.strs;
}

void Buffer::remove(bool removeCurrent) {
  OpData op;
  op.type = OpDelete;
  if(isRegionActive()) {
    ///@todo: what if 'before' is after 'after'!?
    op.before = copyCursors(regions);
    op.after = saveCursors();
    op.strs = removeRegion(op.before, op.after);
    restoreCursors(op.before);
    stopRegion();
  } else if(removeCurrent) {
    op.after = op.before = saveCursors();
    op.strs = removeCurrentChar();
  } else {
    // order is reversed for 'after' and 'before' due to the following
    // insertion op during the undo of this operation!
    op.after = saveCursors();
    op.strs = removeChar();
    op.before = saveCursors();
  }
  lineDown();
  if(!allStringsEmpty(op.strs)) {
    pushNewOp(op);
    modified = true;
  }
}

Strings Buffer::removeChar() {
  Strings del;
  int len = cursorCount();
  int minLoc = getMinStartLoc();
  for(int i=0;i<len;++i) {
    auto& culoc = locs[i];
    if(culoc.x == minLoc && culoc.y == 0) {
      del.push_back("");
      continue;
    }
    if(culoc.x > 0) {
      moveLeftCursorsOnSameLine(i);
      auto str = at(culoc.y).erase(culoc.x, 1);
      del.push_back(str);
      continue;
    }
    int oldy = culoc.y;
    moveUpAllNextCursors(i);
    const auto& oldline = at(oldy);
    auto& newline = at(culoc.y);
    culoc.x = newline.length();
    newline.join(oldline);
    lines.erase(lines.begin()+culoc.y+1);
    del.push_back("\n");
  }
  lineDown();
  // only if there are some chars deleted, the buffer is modified!
  if(!allStringsEmpty(del)) modified = true;
  return del;
}

Strings Buffer::removeRegion(const Positions& start, const Positions& end) {
  Strings del;
  for(size_t i=0;i<start.size();++i)
    del.push_back(removeFrom(start[i], end[i]));
  // only if there are some chars deleted, the buffer is modified!
  if(!allStringsEmpty(del)) modified = true;
  return del;
}

std::string Buffer::removeFrom(const Pos2d<int>& start,
                               const Pos2d<int>& end) {
  std::string del;
  Pos2di small, big;
  if(0 == start.find(small, big, end)) {
    int len = big.x - small.x;
    del = at(small.y).erase(small.x, len);
    return del;
  }
  auto& curr = at(small.y);
  if(small.x == curr.length()) {
    curr.join(at(small.y + 1));
    lines.erase(lines.begin()+small.y+1);
    --big.y;
    del += '\n';
    if(big.y == small.y) {
      del += at(small.y).erase(small.x, big.x);
      return del;
    }
  }
  bool isFullLine = small.x == 0;
  int actualIdx = small.y;
  del += curr.erase(small.x, curr.length()-small.x);
  if(isFullLine)
    lines.erase(lines.begin()+small.y);
  else
    ++actualIdx;
  for(int line=small.y+1;line<big.y;++line) {
    del += '\n';
    del += at(actualIdx).get();
    lines.erase(lines.begin()+actualIdx);
  }
  del += '\n';
  if(big.x > 0) {
    auto& last = at(actualIdx);
    del += last.erase(0, big.x);
  }
  return del;
}

Strings Buffer::removeCurrentChar() {
  Strings del;
  int len = cursorCount();
  for(int i=0;i<len;++i) {
    auto& culoc = locs[i];
    if(culoc.x == lengthOf(culoc.y) && culoc.y == length()-1) {
      del.push_back("");
      continue;
    }
    if(culoc.x < lengthOf(culoc.y)) {
      auto str = lines[culoc.y].erase(culoc.x, 1);
      del.push_back(str);
      continue;
    }
    int y = culoc.y;
    int oldy = y + 1;
    at(y).join(at(oldy));
    lines.erase(lines.begin()+oldy);
    del.push_back("\n");
  }
  // only if there are some chars deleted, the buffer is modified!
  if(!allStringsEmpty(del)) modified = true;
  return del;
}

void Buffer::clear() {
  for(auto& line : lines) line.clear();
  lines.clear();
  addLine();
  startLine = 0;
  begin();
  stopRegion();
  clearStack(undoStack);
  clearStack(redoStack);
}

Strings Buffer::killLine(bool pushToStack) {
  OpData op;
  op.type = OpKillLine;
  stopRegion();
  op.before = saveCursors();
  modified = true;
  int len = cursorCount();
  for(int i=0;i<len;++i) {
    const auto& culoc = locs[i];
    auto& line = lines.at(culoc.y);
    if(culoc.x >= line.length()) {
      if(culoc.y == length()-1) {
        op.strs.push_back("");
        continue;
      }
      auto& next = lines.at(culoc.y + 1);
      line.insert(next.get(), culoc.x);
      lines.erase(lines.begin() + culoc.y + 1);
      op.strs.push_back("\n");
    } else {
      auto str = line.erase(culoc.x, line.length()-culoc.x);
      op.strs.push_back(str);
    }
  }
  op.after = saveCursors();
  if(pushToStack) pushNewOp(op);
  return op.strs;
}
////// End: Buffer editing //////


////// Start: Buffer undo/redo //////
bool Buffer::undo() {
  if(undoStack.empty()) return false;
  auto& top = undoStack.top();
  if(top.type == OpInsert) {
    applyDeleteOp(top);
  } else if(top.type == OpDelete || top.type == OpKillLine) {
    applyInsertOp(top, false);
  }
  redoStack.push(top);
  undoStack.pop();
  return true;
}

bool Buffer::redo() {
  if(redoStack.empty()) return false;
  auto& top = redoStack.top();
  if(top.type == OpInsert) {
    applyInsertOp(top, false);
  } else if(top.type == OpDelete) {
    applyDeleteOp(top);
  } else if(top.type == OpKillLine) {
    restoreCursors(top.before);
    killLine(false);
  }
  undoStack.push(top);
  redoStack.pop();
  return true;
}

void Buffer::applyInsertOp(OpData& op, bool pushToStack) {
  if(pushToStack)
    op.before = saveCursors();
  else
    restoreCursors(op.before);
  forEachCursor([&op, this](Pos2di& cu, size_t idx) {
    auto& str = op.strs[idx];
    for(auto c : str) insert(c, (int)idx);
  });
  modified = true;
  if(pushToStack) {
    op.after = saveCursors();
    pushNewOp(op);
  } else {
    restoreCursors(op.after);
  }
}

void Buffer::applyDeleteOp(OpData& op) {
  restoreCursors(op.before);
  if(op.before == op.after) { // removeCurrent was called
    removeCurrentChar();
  } else {
    removeRegion(op.before, op.after);
  }
  lineDown();
  modified = true;
  restoreCursors(op.before);
}

void Buffer::pushNewOp(OpData& op) {
  if(!disableStack) {
    clearStack(redoStack);
    undoStack.push(op);
  }
}

void Buffer::clearStack(OpStack& st) {
  while(!st.empty()) st.pop();
}
////// End: Buffer undo/redo //////

const AttrColor& Buffer::getColor(const std::string& name) const {
  return mode->getColorMap().get(name);
}

Strings Buffer::regionAsStr() const {
  Strings out;
  if(!isRegionActive()) return out;
  int count = (int)regions.size();
  for(int j=0;j<count;++j) {
    auto rs = regionAsStr(regions[j], locs[j]);
    out.push_back(rs);
  }
  return out;
}

std::string Buffer::regionAsStr(const Pos2di& start, const Pos2di& end) const {
  Pos2di small, big;
  if(0 == start.find(small, big, end)) {
    int len = big.x - small.x;
    const auto& line = at(small.y);
    auto out = line.get().substr(small.x, len);
    return out;
  }
  std::string out = at(small.y).get().substr(small.x);
  for(int i=small.y+1;i<big.y;++i) {
    out += '\n';
    out += at(i).get();
  }
  out += '\n';
  const auto& line = at(big.y);
  out += line.get().substr(0, big.x);
  return out;
}

void Buffer::load(const std::string& file, int line) {
  isDir(file.c_str())? loadDir(file) : loadFile(file, line);
  setMode(Mode::createMode(Mode::inferMode(file)));
}

void Buffer::reload() {
  clear();
  load(fileName, 0);
}

void Buffer::makeReadOnly() {
  setMode(Mode::createMode("ro"));
  modified = false;
  clearStack(undoStack);
  clearStack(redoStack);
}

void Buffer::loadDir(const std::string& dir) {
  Files fs = listDir(dir);
  auto first = rel2abs(pwd(), dir);
  lines.back().append(first);
  for(const auto& f : fs) {
    auto fname = isCurrentOrParentDir(f.name)? f.name : basename(f.name);
    auto buff = format("  %10.10s  %8lu  %s", f.perms, f.size, fname.c_str());
    addLine();
    lines.back().append(buff);
  }
  resetBufferState(0, first);
  begin();
  readOnly = true;
}

void Buffer::loadFile(const std::string& file, int line) {
  auto absFile = rel2abs(pwd(), file);
  std::fstream fp;
  fp.open(absFile.c_str(), std::fstream::in);
  if(!fp.is_open()) {
    resetBufferState(0, absFile);
    return;
  }
  std::string currLine;
  while(std::getline(fp, currLine, '\n')) {
    lines.back().append(currLine);
    addLine();
  }
  fp.close();
  resetBufferState(line, absFile);
  locs[0] = {0, line};
}

void Buffer::resetBufferState(int line, const std::string& file) {
  startLine = line;
  begin();
  modified = false;
  readOnly = isReadOnly(file.c_str());
  fileName = file;
  dirName = isDir(file.c_str())? file : dirname(file);
  buffName = basename(file);
  stopRegion();
}

void Buffer::draw(Editor& ed, const Window& win) {
  const auto& start = win.start();
  const auto& dim = win.dim();
  // draw current buffer (-1 for the status bar)
  int h = start.y + dim.y - 1;
  int len = length();
  const auto& fg = getColor("defaultfg");
  const auto& bg = getColor("defaultbg");
  for(int y=start.y,idx=startLine;y<h&&idx<len;++idx)
    y = drawLine(y, lines[idx].get(), ed, idx, fg, bg, win);
  drawStatusBar(ed, win);
}

void Buffer::drawCursor(Editor& ed, const AttrColor& bg, const Window& win) {
  int n = cursorCount();
  const auto& start = win.start();
  const auto& dim = win.dim();
  const auto& fg = getColor("cursorfg");
  for(int i=0;i<n;++i) {
    auto& culoc = locs[i];
    char c = charAt(culoc);
    auto screenloc = buffer2screen(culoc, start, dim);
    DEBUG("drawCursor: i=%d x,y=%d,%d sloc=%d,%d start=%d\n",
          i, culoc.x, culoc.y, screenloc.x, screenloc.y, startLine);
    ed.sendChar(screenloc.x, screenloc.y, fg, bg, c);
  }
}

void Buffer::drawStatusBar(Editor& ed, const Window& win) {
  int currId = win.currBuffId();
  const auto& start = win.start();
  const auto& dim = win.dim();
  DEBUG("drawStatusBar: dim=%d,%d start=%d,%d\n", dim.x, dim.y, start.x,
        start.y);
  std::string line(dim.x, ' ');
  int x = start.x;
  // -1 is for the status bar
  int y = start.y + dim.y - 1;
  const auto& fg = getColor("statusfg");
  const auto& bg = getColor("statusbg");
  const auto& namefg = getColor("statusnamefg");
  ed.sendString(x, y, fg, bg, line.c_str(), dim.x);
  const auto& loc = locs[0];
  // modified + linenum
  int count = ed.sendStringf(x, y, fg, bg, " %s [%d:%d]/%d ",
                             modified? "**" : "  ", loc.y, loc.x, length());
  // buffer name
  count += ed.sendString(x+count, y, namefg, bg, buffName.c_str(),
                         (int)buffName.length());
  // buffer counts
  count += ed.sendStringf(x+count, y, fg, bg, " <%d/%d> [%s]", currId+1,
                          ed.buffSize(), readOnly? "r-" : "rw");
  // multiple cursor counts
  if(cursorCount() > 1) {
    count += ed.sendStringf(x+count, y, fg, bg, " mc:%d", cursorCount());
  }
  // mode
  count += ed.sendStringf(x+count, y, fg, bg, " [mode=%s]",
                          mode->name().c_str());
}

std::string Buffer::dirModeGetFileAtLine(int line) {
  if(line == 0) return "";
  auto& str = at(line).get();
  return str.substr(dirModeFileOffset());
}

int Buffer::drawLine(int y, const std::string& line, Editor& ed, int lineNum,
                     const AttrColor& fg, const AttrColor& bg,
                     const Window& win) {
  const auto& st = win.start();
  const auto& dim = win.dim();
  int xStart = st.x;
  int wid = dim.x;
  int start = 0;
  int len = (int)line.size();
  // empty line
  if(len <= 0) return y + 1;
  const auto* str = line.c_str();
  bool isD = mode->name() == "dir";
  if(isD) {
    auto file = dirModeGetFileAtLine(lineNum);
    file =  getFileName() + '/' + file;
    isD = isDir(file.c_str());
  }
  while(start < len) {
    int diff = len - start;
    int count = std::min(diff, wid);
    for(int i=0;i<count;++i) {
      // under the highlighted region
      auto c = str[start + i];
      if(regions.isInside(lineNum, start+i, locs)) {
        ed.sendChar(xStart+i, y, getColor("highlightfg"),
                    getColor("highlightbg"), c);
      } else if(isD)
        ed.sendChar(xStart+i, y, getColor("dirfg"), bg, c);
      else
        ed.sendChar(xStart+i, y, fg, bg, c);
    }
    start += wid;
    ++y;
  }
  return y;
}

Pos2di Buffer::buffer2screen(const Pos2di& loc, const Pos2di& start,
                             const Pos2di& dim) const {
  int w = dim.x;
  Pos2d<int> ret = start;
  int relY = loc.y - startLine;
  for(int idx=0;idx<relY;++idx)
    ret.y += lines[idx].numLinesNeeded(w);
  ret.y += (loc.x / w);
  ret.x += loc.x % w;
  return ret;
}

Pos2di Buffer::screen2buffer(const Pos2di& loc, const Pos2di& start,
                             const Pos2di& dim) const {
  Pos2di rel = {loc.x - start.x, loc.y - start.y};
  Pos2di res = {0, startLine};
  int w = dim.x;
  int sy = 0;
  for(;sy<=rel.y;++res.y)
    sy += at(res.y).numLinesNeeded(w);
  if(sy > rel.y) --res.y;
  int dely = rel.y - sy + at(res.y).numLinesNeeded(w);
  res.x = dely * w + rel.x;
  return res;
}

RemovedLines Buffer::keepRemoveLines(Pcre& pc, bool keep) {
  RemovedLines res;
  bool isRegion = isRegionActive();
  for(int i=0;i<length();++i) {
    if(isRegion && !regions.isInside(i, 0, locs)) continue;
    const std::string str = lines[i].get();
    bool match = pc.isMatch(str);
    if((match && keep) || (!match && !keep)) continue;
    lines.erase(lines.begin()+i);
    res.push_back({str, {0, i}});
    --i;
  }
  if(!res.empty()) {
    begin();
    modified = true;
  }
  // ensure that you don't segfault on full buffer removal!
  if(length() <= 0) addLine();
  return res;
}

void Buffer::addLines(const RemovedLines& rlines) {
  // insert from back of the vector to restore the original state!
  for(int i=(int)rlines.size()-1;i>=0;--i) {
    const auto& rl = rlines[i];
    int idx = rl.pos.y;
    lines.insert(lines.begin()+idx, Line());
    lines[idx].append(rl.str);
  }
  begin();
}

bool Buffer::matchCurrentParen() {
  bool isOpen;
  forEachCursor([&isOpen, this](Pos2di& cu, size_t i) {
    bool tmp;
    auto loc = matchCurrentParen((int)i, i==0? isOpen : tmp);
    cu = loc;
  });
  return isOpen;
}

Pos2d<int> Buffer::matchCurrentParen(int i, bool& isOpen) {
  const auto& culoc = locs[i];
  const auto& line = at(culoc.y).get();
  char c = line[culoc.x];
  char mc = getMatchingParen(c);
  std::stack<char> st;
  if(isOpenParen(c)) {
    isOpen = true;
    int len = length();
    for(int y=culoc.y;y<len;++y) {
      int x = (y == culoc.y)? culoc.x : 0;
      int xlen = lengthOf(y);
      const auto& str = at(y).get();
      for(;x<xlen;++x) {
        if(str[x] == c)
          st.push(c);
        else if(str[x] == mc) {
          st.pop();
          if(st.empty()) return {x, y};
        }
      }
    }
  } else if(isCloseParen(c)) {
    isOpen = false;
    for(int y=culoc.y;y>=0;--y) {
      int x = (y == culoc.y)? culoc.x : lengthOf(y);
      const auto& str = at(y).get();
      for(;x>=0;--x) {
        if(str[x] == c)
          st.push(c);
        else if(str[x] == mc) {
          st.pop();
          if(st.empty()) return {x, y};
        }
      }
    }
  }
  return culoc;
}

void Buffer::sortRegions() {
  modified = true;
  int len = cursorCount();
  for(int i=0;i<len;++i) {
    auto& culoc = locs[i];
    int cuy = culoc.y;
    int ry = regions.at(i).y;
    DEBUG("sortRegions: cuy=%d ry=%d i=%d\n", cuy, ry, i);
    sort(lines.begin()+ry, lines.begin()+cuy+1, LineCompare);
    DEBUG("sortRegions: done i=%d\n", i);
    culoc.x = lines[cuy].length();
  }
}

char Buffer::charAt(const Pos2d<int>& pos) const {
  if(pos.y >= length()) return ' ';
  const auto& line = at(pos.y);
  if(pos.x >= line.length()) return ' ';
  return line.at(pos.x);
}

int Buffer::totalLinesNeeded(const Pos2di& dim) const {
  int end = locs[0].y;
  int len = 0;
  for(int i=startLine;i<=end;++i)
    len += lines[i].numLinesNeeded(dim.x);
  return len;
}

void Buffer::lineUp(const Pos2di& dim) {
  // -1 for status bar
  int dimY = dim.y - 1;
  while(totalLinesNeeded(dim) > dimY) ++startLine;
}

void Buffer::lineDown() {
  startLine = std::min(startLine, locs[0].y);
}

void Buffer::lineEnd(const Pos2di& start, const Pos2di& dim) {
  auto screen = buffer2screen(locs[0], start, dim);
  int relY = screen.y - startLine;
  // -1 for the status bar
  int dimY = dim.y - 1;
  if(relY < dimY) return;
  int diff = relY - dimY + 1;
  startLine += diff;
}

bool Buffer::save(const std::string& fName) {
  auto f = fName;
  if(f.empty()) f = fileName;
  if(f.empty()) return false;
  fileName = f;
  std::ofstream fp;
  fp.open(fileName.c_str());
  ASSERT(fp.is_open(), "Failed to open file '%s'!", fileName.c_str());
  int len = (int)lines.size();
  for(int i=0;i<len;++i) {
    // don't write the final line if it is empty
    if(i == len-1 && lines[i].empty()) continue;
    fp << lines[i].get() << "\n";
  }
  fp.close();
  fileName = rel2abs(pwd(), fileName);
  dirName = dirname(fileName);
  buffName = basename(fileName);
  modified = false;
  return true;
}

////// Start: Cursor movements //////
void Buffer::startOfLine() {
  int minLoc = getMinStartLoc();
  forEachCursor([&minLoc](Pos2di& cu, size_t idx) { cu.x = minLoc; });
}

void Buffer::endOfLine() {
  forEachCursor([this](Pos2di& cu, size_t idx) { cu.x = lengthOf(cu.y); });
}

void Buffer::left() {
  int minLoc = getMinStartLoc();
  forEachCursor([&minLoc, this](Pos2di& cu, size_t idx) {
    --cu.x;
    if(cu.x < minLoc) {
      if(cu.y >= 1) {
        --cu.y;
        cu.x = lengthOf(cu.y);
      } else {
        cu.y = 0;
        cu.x = minLoc;
      }
    }
  });
  lineDown();
}

void Buffer::right() {
  forEachCursor([this](Pos2di& cu, size_t idx) {
    ++cu.x;
    if(cu.x > lengthOf(cu.y)) {
      if(cu.y < length()-1) {
        ++cu.y;
        cu.x = 0;
      } else
        --cu.x;
    }
  });
}

void Buffer::down() {
  forEachCursor([this](Pos2di& cu, size_t idx) {
    if(cu.y < length()-1) {
      ++cu.y;
      cu.x = std::min(cu.x, lengthOf(cu.y));
    }
  });
}

void Buffer::up() {
  forEachCursor([this](Pos2di& cu, size_t idx) {
    if(cu.y >= 1) {
      --cu.y;
      cu.x = std::min(cu.x, lengthOf(cu.y));
    }
  });
  lineDown();
}

void Buffer::begin() {
  forEachCursor([](Pos2di& cu, size_t idx) { cu.x = cu.y = 0; });
  lineReset();
}

void Buffer::end() {
  forEachCursor([this](Pos2di& cu, size_t idx) {
    cu.y = std::max(0, length()-1);
    cu.x = lengthOf(cu.y);
  });
}

void Buffer::pageDown(int ijump) {
  forEachCursor([&ijump, this](Pos2di& cu, size_t idx) {
    cu.x = 0;
    cu.y = std::min(length()-1, cu.y+ijump);
  });
}

void Buffer::pageUp(int ijump) {
  forEachCursor([&ijump](Pos2di& cu, size_t idx) {
    cu.x = 0;
    cu.y = std::max(0, cu.y-ijump);
  });
  lineDown();
}

void Buffer::nextPara() {
  int len = length();
  forEachCursor([&len, this](Pos2di& cu, size_t idx) {
    int prevLen = lengthOf(cu.y);
    for(++cu.y;cu.y<len;++cu.y) {
      if(lengthOf(cu.y) == 0 && prevLen != 0) break;
      prevLen = lengthOf(cu.y);
    }
    cu.y = std::min(cu.y, len-1);
    cu.x = 0;
  });
}

void Buffer::previousPara() {
  forEachCursor([this](Pos2di& cu, size_t idx) {
    int prevLen = lengthOf(cu.y);
    for(--cu.y;cu.y>=0;--cu.y) {
      if(lengthOf(cu.y) == 0 && prevLen != 0) break;
      prevLen = lengthOf(cu.y);
    }
    cu.y = std::max(cu.y, 0);
    cu.x = 0;
  });
  lineDown();
}

void Buffer::nextWord() {
  const auto& word = getWord();
  forEachCursor([word, this](Pos2di& cu, size_t idx) {
    const auto& line = at(cu.y);
    if(cu.x >= line.length()) {
      if(cu.y >= length()-1) return;
      ++cu.y;
      cu.x = 0;
    } else {
      cu.x = line.findFirstNotOf(word, cu.x + 1);
    }
  });
}

void Buffer::previousWord() {
  const auto& word = getWord();
  forEachCursor([word, this](Pos2di& cu, size_t idx) {
    if(cu.x <= 0) {
      if(cu.y <= 0) return;
      --cu.y;
      cu.x = lengthOf(cu.y);
    } else {
      const auto& line = at(cu.y);
      cu.x = line.findLastNotOf(word, cu.x - 1);
    }
  });
  lineDown();
}

void Buffer::gotoLine(int lineNum, const Pos2di& dim) {
  forEachCursor([lineNum, this](Pos2di& cu, size_t idx) {
    cu.y = std::min(length()-1, std::max(0, lineNum));
  });
  startLine = std::max(0, lineNum - dim.y/2);
}

void Buffer::removeDuplicateCursors() {
  for(size_t i=0;i<locs.size();++i) {
    for(size_t j=i+1;j<locs.size();++j) {
      if(locs[i] == locs[j]) {
        locs.erase(locs.begin()+j);
        --j;
      }
    }
  }
}
////// End: Cursor movements //////

////// Start: Cursor operations //////
void Buffer::addCursorFromBack(const Pos2di& pos) {
  if(!findCursor(pos)) locs.push_back(pos);
}

void Buffer::addCursorFromFront(const Pos2di& pos) {
  if(!findCursor(pos)) locs.insert(locs.begin(), pos);
}

void Buffer::clearAllCursorsButFirst() {
  if(locs.size() <= 1) return;
  auto first = locs[0];
  locs.clear();
  locs.push_back(first);
}

bool Buffer::hasCursorOn(int line) const {
  for(const auto& cu : locs)
    if(line == cu.y) return true;
  return false;
}

Positions Buffer::copyCursors(const Positions& pos) const {
  Positions out(pos.size());
  std::copy(pos.begin(), pos.end(), out.begin());
  return out;
}

void Buffer::restoreCursors(const Positions& pos) {
  if(pos.empty()) return;
  if(locs.size() != pos.size()) {
    locs.clear();
    locs.resize(pos.size());
  }
  std::copy(pos.begin(), pos.end(), locs.begin());
}

bool Buffer::findCursor(const Pos2di& pos) const {
  for(const auto& cu : locs)
    if(cu == pos) return true;
  return false;
}

void Buffer::moveRightCursorsOnSameLine(int i) {
  const auto culoc = locs[i];
  forEachCursorFrom([&culoc](Pos2di& cu, int idx) {
    if(cu.y == culoc.y) ++cu.x;
  }, i);
}

void Buffer::moveLeftCursorsOnSameLine(int i) {
  const auto culoc = locs[i];
  forEachCursorFrom([&culoc](Pos2di& cu, int idx) {
    if(cu.y == culoc.y) --cu.x;
  }, i);
}

void Buffer::moveDownAllNextCursors(int i) {
  forEachCursorFrom([](Pos2di& cu, int idx) { ++cu.y; }, i);
}

void Buffer::moveUpAllNextCursors(int i) {
  forEachCursorFrom([](Pos2di& cu, int idx) { --cu.y; }, i);
}
////// End: Cursor operations //////

void Buffer::indent() {
  Strings strs;
  Positions start, end;
  bool add = true;
  forEachCursor([&strs, &start, &end, &add, this](Pos2di& cu, int i) {
    int line = cu.y;
    int count = mode->indent(*this, line);
    DEBUG("Indent: count=%d line=%d\n", count, line);
    if(count >= 0) {
      std::string str(count, ' ');
      strs.push_back(str);
      add = true;
    } else {
      start.push_back({0, line});
      end.push_back({-count, line});
      add = false;
    }
  });
  add ? insert(strs) : (void)removeRegion(start, end);
}


Buffers::~Buffers() {
  for(int i=0;i<(int)size();++i) {
    erase(i);
    --i;
  }
  buffNames.clear();
}

void Buffers::clear() {
  std::vector<Buffer*>::clear();
  buffNames.clear();
}

Buffer* Buffers::push_back(const std::string& name) {
  auto uname = uniquify(name);
  auto* buf = new Buffer(uname);
  push_back(buf);
  return buf;
}

void Buffers::push_back(Buffer* buf) {
  std::vector<Buffer*>::push_back(buf);
  buffNames.insert(buf->bufferName());
}

void Buffers::erase(int idx) {
  Buffer* buf = at(idx);
  buffNames.erase(buf->bufferName());
  delete buf;
  std::vector<Buffer*>::erase(begin() + idx);
}

Strings Buffers::namesList() const {
  Strings ret;
  for(const auto* buff : *this) ret.push_back(buff->bufferName());
  return ret;
}

std::string Buffers::uniquify(const std::string& name) const {
  std::string out(name);
  auto start = buffNames.begin(), end = buffNames.end();
  int idx = 0;
  while(std::find(start, end, out) != end) {
    ++idx;
    out = name + "_" + num2str(idx);
  }
  return out;
}

} // end namespace teditor
