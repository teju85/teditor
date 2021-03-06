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
  fileName(), dirName(), tmpFileName(), region(-1, -1),
  mode(Mode::createMode("text")), cu(0, 0), longestX(0), undoStack(),
  redoStack(), disableStack(noUndoRedo) {
  addLine();
  dirName = getpwd();
  begin();
}

////// Start: Buffer editing //////
void Buffer::insert(char c) {
  OpData op;
  op.type = OpInsert;
  op.str = std::string(1, c);
  applyInsertOp(op);
}

void Buffer::insert(const std::string& str) {
  OpData op;
  op.type = OpInsert;
  op.str = str;
  applyInsertOp(op);
}

std::string Buffer::removeAndCopy() {
  OpData op;
  op.type = OpDelete;
  if(!isRegionActive()) return op.str;
  // re-arrange the region to have cursor come after the start
  Point small, big;
  cu.find(small, big, region);
  op.before = small;
  op.after = big;
  op.str = removeRegion(op.before, op.after);
  cu = op.before;
  stopRegion();
  lineDown();
  pushNewOp(op);
  modified = true;
  return op.str;
}

void Buffer::remove(bool removeCurrent) {
  OpData op;
  op.type = OpDelete;
  if(isRegionActive()) {
    // re-arrange the region to have cursor come after the start
    Point small, big;
    cu.find(small, big, region);
    op.before = small;
    op.after = big;
    op.str = removeRegion(op.before, op.after);
    cu = op.before;
    stopRegion();
  } else if(removeCurrent) {
    op.after = op.before = cu;
    op.str = removeCurrentChar();
  } else {
    // order is reversed for 'after' and 'before' due to the following
    // insertion op during the undo of this operation!
    op.after = cu;
    op.str = removeChar();
    op.before = cu;
  }
  if(!op.str.empty()) {
    pushNewOp(op);
    modified = true;
    lineDown();
  }
}

std::string Buffer::removeChar() {
  std::string del;
  int minLoc = getMinStartLoc();
  if(cu.x == minLoc && cu.y == 0) return del;
  if(cu.x > 0) {
    left();
    del = at(cu.y).erase(cu.x, 1);
    return del;
  }
  int oldy = cu.y;
  up();
  const auto& oldline = at(oldy);
  auto& newline = at(cu.y);
  cu.x = newline.length();
  newline.join(oldline);
  lines.erase(lines.begin()+cu.y+1);
  del = "\n";
  lineDown();
  modified = true;
  return del;
}

std::string Buffer::removeRegion(const Point& start, const Point& end) {
  std::string del = removeFrom(start, end);
  // only if there are some chars deleted, the buffer is modified!
  if (!del.empty()) modified = true;
  return del;
}

std::string Buffer::removeFrom(const Point& start, const Point& end) {
  std::string del;
  Point small, big;
  // start == end?
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

std::string Buffer::removeCurrentChar() {
  std::string del;
  if(cu.x == lengthOf(cu.y) && cu.y == length()-1) {
    return del;
  }
  if(cu.x < lengthOf(cu.y)) {
    del = lines[cu.y].erase(cu.x, 1);
    return del;
  }
  int y = cu.y;
  int oldy = y + 1;
  at(y).join(at(oldy));
  lines.erase(lines.begin() + oldy);
  del = "\n";
  modified = true;
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

std::string Buffer::killLine(bool pushToStack) {
  OpData op;
  op.type = OpKillLine;
  stopRegion();
  op.before = cu;
  modified = true;
  auto& line = lines.at(cu.y);
  if(cu.x >= line.length()) {
    if(cu.y == length()-1) {
      op.str = "";
    } else {
      auto& next = lines.at(cu.y + 1);
      line.insert(next.get(), cu.x);
      lines.erase(lines.begin() + cu.y + 1);
      op.str = "\n";
    }
  } else {
    op.str = line.erase(cu.x, line.length() - cu.x);
  }
  op.after = cu;
  if(pushToStack) pushNewOp(op);
  return op.str;
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
  } else if(top.type == OpKeepRemoveLines) {
    // in case of full removal!
    if (length() == 1 && lengthOf(length() - 1) == 0) {
      lines.erase(lines.begin());
    }
    addLines(top.rlines);
    cu = top.before;
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
    cu = top.before;
    killLine(false);
  } else if(top.type == OpKeepRemoveLines) {
    removeLines(top.rlines);
    cu = top.before;
  }
  undoStack.push(top);
  redoStack.pop();
  return true;
}

void Buffer::applyInsertOp(OpData& op, bool pushToStack) {
  if(pushToStack)
    op.before = cu;
  else
    cu = op.before;
  for (auto c : op.str) insertImpl(c);
  modified = true;
  if(pushToStack) {
    op.after = cu;
    pushNewOp(op);
  } else {
    cu = op.after;
  }
}

///@todo: what if a single line crosses the whole screen!?
void Buffer::insertImpl(char c) {
  if(c == '\n' || c == (char)Key_Enter) {
    auto newLine = at(cu.y).split(cu.x);
    lines.insert(lines.begin() + cu.y + 1, newLine);
    cu.x = 0;
    ++cu.y;
    return;
  }
  auto& line = lines[cu.y];
  line.insert(c, cu.x);
  right();
}

void Buffer::applyDeleteOp(OpData& op) {
  cu = op.before;
  if(op.before == op.after) { // removeCurrent was called
    removeCurrentChar();
  } else {
    removeRegion(op.before, op.after);
  }
  lineDown();
  modified = true;
  cu = op.before;
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

std::string Buffer::regionAsStr() const {
  std::string out;
  if(!isRegionActive()) return out;
  out = regionAsStr(region, cu);
  return out;
}

std::string Buffer::regionAsStr(const Point& start, const Point& end) const {
  Point small, big;
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
  ASSERT(isAbs(file), "Buffer::load file must be abs-path! '%s'", file.c_str());
  bool dir = isDir(file);
  dir ? loadDir(file) : loadFile(file, line);
  setMode(Mode::createMode(Mode::inferMode(file, dir)));
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
  insert(dir + "\n" + listDir2str(dir));
  resetBufferState(0, dir, true);
  readOnly = true;
}

void Buffer::loadFile(const std::string& file, int line) {
  std::string inFile;
  if (isRemote(file)) {
    inFile = tmpFileName = copyFromRemote(file);
  } else {
    inFile = file;
  }
  std::fstream fp;
  fp.open(inFile.c_str(), std::fstream::in);
  if(fp.is_open()) {
    std::string currLine;
    while(std::getline(fp, currLine, '\n')) {
      lines.back().append(currLine);
      addLine();
    }
    fp.close();
  }
  line = std::min(std::max(0, line), length() - 1);
  resetBufferState(line, file, false);
  cu = {0, line};
}

void Buffer::resetBufferState(int line, const std::string& file, bool dir) {
  startLine = line;
  begin();
  modified = false;
  readOnly = isReadOnly(file.c_str());
  fileName = file;
  dirName = dir ? file : dirname(file);
  buffName = basename(file);
  stopRegion();
}

void Buffer::draw(Editor& ed, const Window& win) {
  const auto& start = win.start();
  const auto& dim = win.dim();
  // draw current buffer (-1 for the status bar)
  int h = start.y + dim.y - 1;
  int len = length();
  for(int y = start.y, idx = startLine; y < h && idx < len; ++idx)
    y = drawLine(y, lines[idx].get(), ed, idx, win);
  drawStatusBar(ed, win);
}

void Buffer::drawPoint(Editor& ed, const AttrColor& bg, const Window& win) {
  const auto& start = win.start();
  const auto& dim = win.dim();
  const auto& fg = getColor("cursorfg");
  char c = charAt(cu);
  auto screenloc = buffer2screen(cu, start, dim);
  DEBUG("drawPoint: x,y=%d,%d sloc=%d,%d start=%d\n", cu.x, cu.y, screenloc.x,
        screenloc.y, startLine);
  ed.sendChar(screenloc.x, screenloc.y, fg, bg, c);
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
  int count = 0;
  // mode
  count += ed.sendStringf(x+count, y, fg, bg, " [mode=%s]",
                          mode->name().c_str());
  // buffer counts
  count += ed.sendStringf(x+count, y, fg, bg, " <%d/%d> [%s] %s", currId+1,
                          ed.buffSize(), readOnly? "r-" : "rw",
                          isRemote(fileName) ? "[ssh] " : "");
  // buffer name
  count += ed.sendString(x+count, y, namefg, bg, buffName.c_str(),
                         (int)buffName.length());
  // modified + linenum
  count += ed.sendStringf(x+count, y, fg, bg, " %s [%d:%d]/%d ",
                          modified? "**" : "  ", cu.y, cu.x, length());
}

std::string Buffer::dirModeGetFileAtLine(int line) {
  if(line == 0) return "";
  auto& str = at(line).get();
  if (str.empty()) return "";
  return str.substr(dirModeFileOffset());
}

int Buffer::drawLine(int y, const std::string& line, Editor& ed, int lineNum,
                     const Window& win) {
  const auto& st = win.start();
  const auto& dim = win.dim();
  int xStart = st.x, wid = dim.x, start = 0;
  int len = (int)line.size();
  const auto* str = line.c_str();
  ULTRA_DEBUG("Buffer::drawLine: y=%d line=%s len=%d\n", y, line.c_str(), len);
  auto maxLen = std::max(len, wid);
  while(start < maxLen) {
    int diff = maxLen - start;
    int count = std::min(diff, wid);
    for(int i = 0; i < count; ++i) {
      // under the highlighted region
      auto c = start + i < len ? str[start + i] : ' ';
      auto highlighted = region.isInside(lineNum, start + i, cu);
      AttrColor fg, bg;
      mode->getColorFor(fg, bg, lineNum, start + i, *this, highlighted);
      ed.sendChar(xStart + i, y, fg, bg, c);
    }
    start += wid;
    ++y;
  }
  ULTRA_DEBUG("Buffer::drawLine: ended y=%d line=%s\n", y, line.c_str());
  return y;
}

Point Buffer::buffer2screen(const Point& loc, const Point& start,
                            const Point& dim) const {
  int w = dim.x;
  Point ret = start;
  int relY = loc.y - startLine;
  for(int idx=0;idx<relY;++idx) ret.y += lines[idx].numLinesNeeded(w);
  ret.y += (loc.x / w);
  ret.x += loc.x % w;
  return ret;
}

Point Buffer::screen2buffer(const Point& loc, const Point& start,
                            const Point& dim) const {
  Point rel = {loc.x - start.x, loc.y - start.y};
  Point res = {0, startLine};
  int w = dim.x, sy = 0;
  for(;sy<=rel.y;++res.y) sy += at(res.y).numLinesNeeded(w);
  if(sy > rel.y) --res.y;
  int dely = rel.y - sy + at(res.y).numLinesNeeded(w);
  res.x = dely * w + rel.x;
  return res;
}

void Buffer::keepRemoveLines(parser::NFA& regex, bool keep) {
  OpData op;
  op.type = OpKeepRemoveLines;
  op.after = {0, 0};
  op.before = cu;
  Point small, big;
  if (isRegionActive()) {
    cu.find(small, big, region);
    stopRegion();
  } else {
    small = {0, cu.y};
    big = {0, length() - 1};
  }
  for(int i = small.y; i <= big.y; ++i) {
    auto str = at(i).get();
    size_t tmp;
    bool match = regex.findAny(str, tmp) != parser::NFA::NoMatch;
    if((match && keep) || (!match && !keep)) continue;
    lines.erase(lines.begin() + i);
    op.rlines.push_back({str, i});
    --big.y;
    --i;
  }
  if(!op.rlines.empty()) {
    begin();
    modified = true;
    pushNewOp(op);
  }
  // ensure that you don't segfault on full buffer removal!
  if(length() <= 0) addLine();
}

void Buffer::addLines(const RemovedLines& rlines) {
  // insert from back of the vector to restore the original state!
  for(int i = (int)rlines.size() - 1; i >= 0; --i) {
    const auto& rl = rlines[i];
    lines.insert(lines.begin() + rl.num, Line());
    lines[rl.num].append(rl.str);
  }
}

void Buffer::removeLines(const RemovedLines& rlines) {
  // remove from back of the vector to restore the original state!
  for(int i = (int)rlines.size() - 1; i >= 0; --i) {
    const auto& rl = rlines[i];
    lines.erase(lines.begin() + rl.num);
  }
  // ensure that you don't segfault on full buffer removal!
  if(length() <= 0) addLine();
}

bool Buffer::matchCurrentParen() {
  bool isOpen;
  cu = matchCurrentParen(isOpen);
  return isOpen;
}

Point Buffer::matchCurrentParen(bool& isOpen) {
  const auto& line = at(cu.y).get();
  char c = line[cu.x];
  char mc = getMatchingParen(c);
  std::stack<char> st;
  if(isOpenParen(c)) {
    isOpen = true;
    int len = length();
    for(int y=cu.y;y<len;++y) {
      int x = (y == cu.y)? cu.x : 0;
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
    for(int y=cu.y;y>=0;--y) {
      int x = (y == cu.y)? cu.x : lengthOf(y);
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
  return cu;
}

void Buffer::sortRegion() {
  modified = true;
  int cuy = cu.y;
  int ry = region.y;
  DEBUG("sortRegion: cuy=%d ry=%d\n", cuy, ry);
  std::sort(lines.begin()+ry, lines.begin()+cuy+1, LineCompare);
  DEBUG("sortRegion: done\n");
  cu.x = lines[cuy].length();
}

char Buffer::charAt(const Point& pos) const {
  if(pos.y >= length()) return ' ';
  const auto& line = at(pos.y);
  if(pos.x >= line.length()) return ' ';
  return line.at(pos.x);
}

int Buffer::totalLinesNeeded(const Point& dim) const {
  int end = cu.y;
  int len = 0;
  for(int i = startLine; i <= end; ++i) len += lines[i].numLinesNeeded(dim.x);
  return len;
}

void Buffer::lineUp(const Point& dim) {
  // -1 for status bar
  int dimY = dim.y - 1;
  while(totalLinesNeeded(dim) > dimY) ++startLine;
}

void Buffer::lineDown() { startLine = std::min(startLine, cu.y); }

void Buffer::lineEnd(const Point& start, const Point& dim) {
  auto screen = buffer2screen(cu, start, dim);
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
  std::string outFile = fileName = f;
  if (isRemote(fileName)) {
    if (tmpFileName.empty())
      tmpFileName = tempFileName();
    outFile = tmpFileName;
  }
  std::ofstream fp;
  fp.open(outFile.c_str());
  ASSERT(fp.is_open(), "Failed to open file '%s'!", outFile.c_str());
  int len = (int)lines.size();
  for(int i=0;i<len;++i) {
    // don't write the final line if it is empty
    if(i == len-1 && lines[i].empty()) continue;
    fp << lines[i].get() << "\n";
  }
  fp.close();
  if (!isRemote(fileName)) {
    fileName = rel2abs(pwd(), fileName);
  } else {
    copyToRemote(fileName, outFile);
  }
  dirName = dirname(fileName);
  buffName = basename(fileName);
  modified = false;
  return true;
}

////// Start: Cursor movements //////
void Buffer::left() {
  int minLoc = getMinStartLoc();
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
  lineDown();
  longestX = cu.x;
}

void Buffer::right() {
  ++cu.x;
  if(cu.x > lengthOf(cu.y)) {
    if(cu.y < length()-1) {
      ++cu.y;
      cu.x = 0;
    } else
      --cu.x;
  }
  longestX = cu.x;
}

void Buffer::down() {
  if(cu.y < length()-1) {
    ++cu.y;
    cu.x = std::min(lengthOf(cu.y), longestX);
  }
}

void Buffer::up() {
  if(cu.y >= 1) {
    --cu.y;
    cu.x = std::min(lengthOf(cu.y), longestX);
  }
  lineDown();
}

void Buffer::begin() {
  longestX = cu.x = cu.y = 0;
  lineReset();
}

void Buffer::end() {
  cu.y = std::max(0, length() - 1);
  longestX = cu.x = lengthOf(cu.y);
}

void Buffer::pageDown(int ijump) {
  cu.y = std::min(length() - 1, cu.y + ijump);
  cu.x = std::min(lengthOf(cu.y), longestX);
}

void Buffer::pageUp(int ijump) {
  cu.y = std::max(0, cu.y - ijump);
  lineDown();
  cu.x = std::min(lengthOf(cu.y), longestX);
}

void Buffer::nextPara() {
  int len = length();
  int prevLen = lengthOf(cu.y);
  for(++cu.y; cu.y < len; ++cu.y) {
    if(lengthOf(cu.y) == 0 && prevLen != 0) break;
    prevLen = lengthOf(cu.y);
  }
  cu.y = std::min(cu.y, len-1);
  cu.x = std::min(lengthOf(cu.y), longestX);
}

void Buffer::previousPara() {
  int prevLen = lengthOf(cu.y);
  for(--cu.y; cu.y >= 0; --cu.y) {
    if(lengthOf(cu.y) == 0 && prevLen != 0) break;
    prevLen = lengthOf(cu.y);
  }
  cu.y = std::max(cu.y, 0);
  lineDown();
  cu.x = std::min(lengthOf(cu.y), longestX);
}

void Buffer::nextWord() {
  const auto& word = getWord();
  while (true) {
    const auto& line = at(cu.y);
    if (cu.x >= line.length()) {
      if (cu.y >= length() - 1) return;
      ++cu.y;
      cu.x = 0;
      continue;
    }
    if (word.find(line.at(cu.x)) == std::string::npos) {
      ++cu.x;
      continue;
    }
    cu.x = line.findFirstNotOf(word, cu.x + 1);
    break;
  }
  longestX = cu.x;
}

void Buffer::previousWord() {
  const auto& word = getWord();
  while (true) {
    const auto& line = at(cu.y);
    if (cu.x <= 0) {
      if (cu.y <= 0) return;
      --cu.y;
      cu.x = lengthOf(cu.y);
      continue;
    }
    if (word.find(line.at(cu.x)) == std::string::npos) {
      --cu.x;
      continue;
    }
    cu.x = line.findLastNotOf(word, cu.x - 1);
    break;
  }
  lineDown();
  longestX = cu.x;
}

void Buffer::gotoLine(int lineNum, const Point& dim) {
  cu.y = std::min(length() - 1, std::max(0, lineNum));
  startLine = std::max(0, lineNum - dim.y / 2);
  longestX = cu.x = 0;
}
////// End: Cursor movements //////

void Buffer::indent() {
  int line = cu.y;
  int count = mode->indent(*this, line);
  DEBUG("Indent: count=%d line=%d\n", count, line);
  if (count == 0) return;
  startOfLine();
  if(count > 0) {
    auto str = std::string(count, ' ');
    insert(str);
  } else {
    startRegion();
    cu.x = -count;
    remove();
  }
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

Buffer* Buffers::push_back(const std::string& name, bool noUndoRedo) {
  auto uname = uniquify(name);
  auto* buf = new Buffer(uname, noUndoRedo);
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
