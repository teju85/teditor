#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for wcstring, strcasestr
#endif

#include "buffer.h"
#include <cstring>
#include <unistd.h>
#include <wchar.h>
#include "editor.h"
#include "logger.h"
#include <iostream>
#include <fstream>
#include "gtest/gtest_prod.h"
#include <unistd.h>
#include <algorithm>
#include <stack>


namespace teditor {

ByteBuffer::ByteBuffer(int cap): std::string() {
    reserve(cap);
}

void ByteBuffer::reserve(int cap) {
    int bcap = capacity();
    if(bcap >= cap)
        return;
    // prefer doubling capacity
    if(bcap*2 > cap)
        cap = bcap * 2;
    std::string::reserve(cap);
}

void ByteBuffer::append(const char* data, int len) {
    reserve(length()+len);
    std::string::append(data, len);
}

void ByteBuffer::puts(const char* data) {
    append(data, strlen(data));
}

void ByteBuffer::flush(int fd) {
    write(fd, c_str(), length());
    //DEBUG("flush: len=%d buff=%s\n", length(), c_str());
    clear();
}


void Cell::copy(const Cell& from) {
    ch = from.ch;
    fg = from.fg;
    bg = from.bg;
}

void Cell::set(Chr c, AttrColor f, AttrColor b) {
    ch = c;
    fg = f;
    bg = b;
}

void Cell::set(Chr c, color_t f, color_t b) {
    ch = c;
    fg = f;
    bg = b;
}

int Cell::width() const {
    return wcwidth(ch);
}

bool operator==(const Cell& a, const Cell& b) {
    return (a.ch == b.ch) && (a.fg == b.fg) && (a.bg == b.bg);
}

CellBuffer::CellBuffer(int w, int h): width(w), height(h), cells(w*h) {
}

void CellBuffer::clear(AttrColor fg, AttrColor bg) {
    int len = width * height;
    for(int i=0;i<len;++i) {
        cells[i].set(' ', fg, bg);
    }
}

void CellBuffer::resize(int w, int h) {
    if((width == w) && (height == h))
        return;
    int oldw=width, oldh=height;
    std::vector<Cell> newCells(w*h);
    int minw = (w < oldw) ? w : oldw;
    int minh = (h < oldh) ? h : oldh;
    for(int i=0;i<minh;++i) {
        for(int j=0;j<minw;++j) {
            newCells[i*width+j] = cells[i*oldw+j];
        }
    }
    cells = newCells;
    width = w;
    height = h;
}


std::string Line::erase(int idx, int count) {
    std::string ret;
    if(idx >= length() || (idx+count) > length())
        return ret;
    ret = line.substr(idx, count);
    line.erase(line.begin()+idx, line.begin()+idx+count);
    return ret;
}

void Line::insert(char c, int idx) {
    if(idx >= length())
        append(c);
    else
        line.insert(line.begin()+idx, c);
}

void Line::insert(const char* c, int idx) {
    if(idx >= length())
        append(c);
    else
        line.insert(idx, c);
}

int Line::numLinesNeeded(int wid) const {
    int len = length();
    // an empty line is still a line!
    if(len <= 0)
        return 1;
    return (len + wid - 1) / wid;
}


void Regions::enable(const Positions& p) {
    for(const auto& l : p)
        locs.push_back(l);
}

bool Regions::isInside(int y, int x, const Cursor& cu, int i) const {
    const auto& p = locs[i];
    const auto& culoc = cu.at(i);
    int yStart, yEnd, xStart, xEnd;
    if(p.y < culoc.y) {
        yStart = p.y;
        yEnd = culoc.y;
        xStart = p.x;
        xEnd = culoc.x;
    } else if(p.y > culoc.y) {
        yStart = culoc.y;
        yEnd = p.y;
        xStart = culoc.x;
        xEnd = p.x;
    } else {
        yStart = p.y;
        yEnd = culoc.y;
        xStart = std::min(p.x, culoc.x);
        xEnd = std::max(p.x, culoc.x);
    }
    if(yStart < y && y < yEnd)
        return true;
    else if(yStart == yEnd && yStart == y) {
        if(xStart <= x && x <= xEnd)
            return true;
    } else if(yStart == y && x >= xStart)
        return true;
    else if(yEnd == y && x <= xEnd)
        return true;
    return false;
}

bool Regions::isInside(int y, int x, const Cursor& cu) const {
    if(locs.empty())
        return false;
    int len = cu.count();
    for(int i=0;i<len;++i)
        if(isInside(y, x, cu, i))
            return true;
    return false;
}


MultiLine::MultiLine(const std::string& name):
    screenStart(), screenDim(), lines(), startLine(0), cursor(),
    modified(false), readOnly(false), buffName(name), fileName(), dirName(),
    regions(), regionActive(false), kcMap(), cMap(), cmds(), topCmd(-1),
    word("abcdefghijklmnopqrstuvwxyzABCDEGGHIJKLMNOPQRSTUVWXYZ0123456789_") {
    addLine();
    cursor.reset(this);
    dirName = getpwd();
    populateKeyMap<GlobalKeys>(kcMap);
    populateColorMap<GlobalColors>(cMap);
    kcMap.resetTraversal();
}

MultiLine::~MultiLine() {
    if(!fileName.empty() && Editor::instanceReady()) {
        auto& ed = Editor::getInstance();
        int line = cursor.at(0).y;
        ed.addFileHistory(fileName, line);
    }
}

const AttrColor& MultiLine::getColor(const std::string& name) const {
    return cMap.get(name);
}

void MultiLine::addCommand(CmdPtr c) {
    int len = (int)cmds.size();
    if(topCmd < 0) {
        cmds.clear();
        cmds.push_back(c);
    } else if(topCmd == len-1) {
        cmds.push_back(c);
    } else {
        cmds.erase(cmds.begin()+topCmd+1, cmds.end());
        cmds.push_back(c);
    }
    topCmd = (int)cmds.size() - 1;
}

void MultiLine::undo() {
    if(topCmd < 0) {
        CMBAR_MSG("No further undo!");
        return;
    }
    cmds[topCmd]->exec(CMD_UNDO);
    --topCmd;
}

void MultiLine::redo() {
    int len = (int)cmds.size();
    if(topCmd >= len-1) {
        CMBAR_MSG("No further redo!");
        return;
    }
    topCmd++;
    cmds[topCmd]->exec(CMD_REDO);
}

std::vector<std::string> MultiLine::regionAsStr() const {
    std::vector<std::string> out;
    if(!isRegionActive())
        return out;
    int count = regions.count();
    for(int j=0;j<count;++j) {
        std::string rs;
        const auto& p = regions.at(j);
        const auto& culoc = cursor.at(j);
        int yStart, yEnd, xStart, xEnd;
        if(p.y < culoc.y) {
            yStart = p.y;
            yEnd = culoc.y;
            xStart = p.x;
            xEnd = culoc.x;
        } else if(p.y > culoc.y) {
            yStart = culoc.y;
            yEnd = p.y;
            xStart = culoc.x;
            xEnd = p.x;
        } else {
            yStart = p.y;
            yEnd = culoc.y;
            xStart = std::min(p.x, culoc.x);
            xEnd = std::max(p.x, culoc.x);
            int len = xEnd - xStart;
            const auto& str = at(yStart).get();
            rs = str.substr(xStart, len);
            out.push_back(rs);
            continue;
        }
        rs = at(yStart).get().substr(xStart);
        for(int i=yStart+1;i<yEnd;++i) {
            rs += '\n';
            rs += at(i).get();
        }
        rs += '\n';
        rs += at(yEnd).get().substr(0, xEnd);
        out.push_back(rs);
    }
    return out;
}

void MultiLine::enableRegions() {
    regionActive = true;
    regions.enable(cursor.getLocs());
}

void MultiLine::disableRegions() {
    regionActive = false;
    regions.clear();
}

void MultiLine::resize(const Pos2d<int>& start, const Pos2d<int>& dim) {
    screenStart = start;
    screenDim = dim;
    // last line is kept for status bar for this buffer
    screenDim.y -= 1;
}

void MultiLine::load(const std::string& file, int line) {
    if(isDir(file.c_str())) {
        loadDir(file);
        return;
    }
    loadFile(file, line);
}

void MultiLine::reload() {
    clear();
    load(fileName, 0);
}

void MultiLine::loadDir(const std::string& dir) {
    Files fs = listDir(dir);
    auto first = rel2abs(pwd(), dir);
    lines.at(length()-1).append(first.c_str());
    for(const auto& f : fs) {
        auto fname = (f.name == "." || f.name == "..")? f.name : basename(f.name);
        char buff[1024];
        sprintf(buff, "  %10.10s  %8lu  %s", f.perms, f.size, fname.c_str());
        addLine();
        lines.at(length()-1).append(buff);
    }
    resetBufferState(0, first);
    cursor.reset(this);
    readOnly = true;
    populateKeyMap<DirModeKeys>(kcMap);
}

void MultiLine::loadFile(const std::string& file, int line) {
    auto absFile = rel2abs(pwd(), file);
    std::fstream fp;
    fp.open(absFile.c_str(), std::fstream::in);
    if(!fp.is_open()) {
        resetBufferState(0, absFile);
        return;
    }
    std::string currLine;
    while(std::getline(fp, currLine, '\n')) {
        lines.at(length()-1).append(currLine.c_str());
        addLine();
    }
    fp.close();
    resetBufferState(line, absFile);
    cursor.at(0) = {0, line};
}

void MultiLine::resetBufferState(int line, const std::string& file) {
    startLine = line;
    cursor.reset(this);
    modified = false;
    readOnly = isReadOnly(file.c_str());
    fileName = file;
    dirName = isDir(file.c_str())? file : dirname(file);
    buffName = basename(file);
    disableRegions();
    cmds.clear();
    topCmd = -1;
}

void MultiLine::drawBuffer(Editor& ed) {
    // draw current buffer
    int h = screenStart.y + screenDim.y;
    int len = length();
    for(int y=screenStart.y,idx=startLine;y<h&&idx<len;++idx)
        y = drawLine(y, lines[idx].get(), ed, idx, "defaultfg", "defaultbg");
}

void MultiLine::drawCursor(Editor& ed, const std::string& bg) {
    int n = cursor.count();
    for(int i=0;i<n;++i) {
        auto& culoc = cursor.at(i);
        char c = charAt(culoc);
        auto screenloc = buffer2screen(culoc);
        ed.sendChar(screenloc.x, screenloc.y, "cursorfg", bg, c);
    }
}

void MultiLine::drawStatusBar(Editor& ed) {
    std::string line(screenDim.x, ' ');
    int x = screenStart.x;
    int y = screenStart.x + screenDim.y;
    ed.sendString(x, y, "statusfg", "statusbg", line.c_str(), screenDim.x);
    const auto& loc = cursor.at(0);
    // modified + linenum
    int count = ed.sendStringf(x, y, "statusfg", "statusbg",
                               " %s [%d:%d]/%d ", modified? "**" : "  ",
                               loc.y, loc.x, length());
    // buffer name
    count += ed.sendString(x+count, y, "statusnamefg", "statusbg",
                           buffName.c_str(), (int)buffName.length());
    // buffer counts
    count += ed.sendStringf(x+count, y, "statusfg", "statusbg",
                            " <%d/%d> [%s]", ed.currBuffId()+1, ed.buffSize(),
                            readOnly? "r-" : "rw");
    // multiple cursor counts
    if(cursor.count() > 1) {
        ed.sendStringf(x+count, y, "statusfg", "statusbg",
                       " mc:%d", cursor.count());
    }
}

int MultiLine::drawLine(int y, const std::string& line, Editor& ed, int lineNum,
                        const std::string& fg, const std::string& bg) {
    int xStart = screenStart.x;
    int wid = screenDim.x;
    int start = 0;
    int len = (int)line.size();
    // empty line
    if(len <= 0)
        return y + 1;
    const auto* str = line.c_str();
    while(start < len) {
        int diff = len - start;
        int count = std::min(diff, wid);
        for(int i=0;i<count;++i) {
            // under the highlighted region
            auto c = str[start + i];
            if(regions.isInside(lineNum, start+i, cursor)) {
                ed.sendChar(xStart+i, y, "highlightfg", "highlightbg", c);
            } else {
                ed.sendChar(xStart+i, y, fg, bg, c);
            }
        }
        start += wid;
        ++y;
    }
    return y;
}

Pos2d<int> MultiLine::buffer2screen(const Pos2d<int>& loc) const {
    int w = screenDim.x;
    Pos2d<int> ret = screenStart;
    int relY = loc.y - startLine;
    for(int idx=0;idx<relY;++idx) {
        ret.y += lines[idx].numLinesNeeded(w);
    }
    ret.y += (loc.x / w);
    ret.x += loc.x % w;
    return ret;
}

void MultiLine::insertLine(int i) {
    const auto& pos = cursor.at(i);
    lines.insert(lines.begin()+pos.y+1, Line());
    auto& oldline = lines[pos.y];
    bool eol = pos.x >= oldline.length();
    if(!eol) {
        std::string res = oldline.erase(pos.x, oldline.length()-pos.x);
        auto& newline = lines[pos.y+1];
        newline.append(res.c_str());
    }
    // inserting line at the current cursor means all other cursors below it
    // must also be moved down by one line to preserve their right location
    cursor.moveDownAllNextCursors(i);
    cursor.at(i).x = 0;
}

void MultiLine::insert(char c) {
    modified = true;
    int len = cursor.count();
    for(int i=0;i<len;++i) {
        insert(c, i);
    }
    lineUp();
    cursor.removeDuplicates();
}

///@todo: what if a single line crosses the whole screen!?
void MultiLine::insert(char c, int i) {
    if(c == '\n' || c == (char)Key_Enter) {
        insertLine(i);
        return;
    }
    auto& culoc = cursor.at(i);
    auto& line = lines[culoc.y];
    line.insert(c, culoc.x);
    // inserting a char should move other cursors in the same line!
    cursor.moveRightCursorsOnSameLine(i);
}

void MultiLine::insert(const std::vector<std::string>& strs) {
    ASSERT((int)strs.size() == cursor.count(),
           "insert: lines passed not the same as number of cursors! [%d,%d]",
           (int)strs.size(), cursor.count());
    int len = cursor.count();
    for(int i=0;i<len;++i) {
        auto& str = strs[i];
        for(auto c : str)
            insert(c, i);
    }
}

void MultiLine::insert(const char* buf) {
    int len = strlen(buf);
    for(int i=0;i<len;++i) {
        insert(buf[i]);
    }
}

RemovedLines MultiLine::keepRemoveLines(Pcre& pc, bool keep) {
    RemovedLines res;
    bool isRegion = isRegionActive();
    for(int i=0;i<(int)lines.size();++i) {
        if(isRegion && !regions.isInside(i, 0, cursor))
            continue;
        const std::string str = lines[i].get();
        bool match = pc.isMatch(str);
        if(match && keep)
            continue;
        if(!match && !keep)
            continue;
        lines.erase(lines.begin()+i);
        res.push_back({str, {0, i}});
        --i;
    }
    if(!res.empty()) {
        cursor.reset(this);
        modified = true;
    }
    return res;
}

void MultiLine::addLines(const RemovedLines& rlines) {
    // insert from back of the vector to restore the original state!
    for(int i=(int)rlines.size()-1;i>=0;--i) {
        const auto& rl = rlines[i];
        int idx = rl.pos.y;
        lines.insert(lines.begin()+idx, Line());
        lines[idx].append(rl.str);
    }
    cursor.reset(this);
}

void MultiLine::gotoLine(int lineNum) {
    int len = cursor.count();
    for(int i=0;i<len;++i) {
        auto& culoc = cursor.at(i);
        culoc.y = std::min((int)lines.size()-1, std::max(0, lineNum));
    }
    cursor.removeDuplicates();
}

void MultiLine::matchCurrentParen() {
    int len = cursor.count();
    bool isOpen;
    for(int i=0;i<len;++i) {
        bool tmp;
        auto loc = matchCurrentParen(i, i==0? isOpen : tmp);
        cursor.at(i) = loc;
    }
    if(isOpen)
        lineUp();
    else
        lineDown();
    cursor.removeDuplicates();
}

Pos2d<int> MultiLine::matchCurrentParen(int i, bool& isOpen) {
    const auto& culoc = cursor.at(i);
    const auto& line = at(culoc.y).get();
    char c = line[culoc.x];
    char mc = getMatchingParen(c);
    std::stack<char> st;
    if(isOpenParen(c)) {
        isOpen = true;
        int len = length();
        for(int y=culoc.y;y<len;++y) {
            int x = (y == culoc.y)? culoc.x : 0;
            int xlen = at(y).length();
            const auto& str = at(y).get();
            for(;x<xlen;++x) {
                if(str[x] == c)
                    st.push(c);
                else if(str[x] == mc) {
                    st.pop();
                    if(st.empty())
                        return {x, y};
                }
            }
        }
    } else if(isCloseParen(c)) {
        isOpen = false;
        for(int y=culoc.y;y>=0;--y) {
            int x = (y == culoc.y)? culoc.x : at(y).length();
            const auto& str = at(y).get();
            for(;x>=0;--x) {
                if(str[x] == c)
                    st.push(c);
                else if(str[x] == mc) {
                    st.pop();
                    if(st.empty())
                        return {x, y};
                }
            }
        }
    }
    return culoc;
}

std::vector<std::string> MultiLine::remove() {
    std::vector<std::string> del;
    modified = true;
    int len = cursor.count();
    int minLoc = getMinStartLoc();
    for(int i=0;i<len;++i) {
        auto& culoc = cursor.at(i);
        if(culoc.x == minLoc && culoc.y == 0) {
            del.push_back("");
            continue;
        }
        if(culoc.x > 0) {
            cursor.moveLeftCursorsOnSameLine(i);
            auto str = lines[culoc.y].erase(culoc.x, 1);
            del.push_back(str);
            continue;
        }
        int oldy = culoc.y;
        const auto& oldstr = lines[oldy].get();
        cursor.moveUpAllNextCursors(i);
        auto& newline = lines[culoc.y];
        culoc.x = newline.length();
        newline.insert(oldstr.c_str(), culoc.x);
        lines.erase(lines.begin()+culoc.y+1);
        del.push_back("\n");
    }
    lineDown();
    return del;
}

std::vector<std::string> MultiLine::remove(const Positions& start,
                                           const Positions& end) {
    modified = true;
    ASSERT(start.size() == end.size(),
           "remove: start length not same as end's [%lu,%lu]\n",
           start.size(), end.size());
    std::vector<std::string> del;
    int len = (int)start.size();
    for(int i=0;i<len;++i)
        del.push_back(removeFrom(start[i], end[i]));
    return del;
}

std::string MultiLine::removeFrom(const Pos2d<int>& start,
                                  const Pos2d<int>& end) {
    std::string del;
    int yStart, yEnd, xStart, xEnd;
    if(start.y < end.y) {
        yStart = start.y;
        yEnd = end.y;
        xStart = start.x;
        xEnd = end.x;
    } else if(start.y > end.y) {
        yStart = end.y;
        yEnd = start.y;
        xStart = start.x;
        xEnd = end.x;
    } else {
        yStart = start.y;
        yEnd = end.y;
        xStart = std::min(start.x, end.x);
        xEnd = std::max(start.x, end.x);
        int len = xEnd - xStart;
        del = at(yStart).erase(xStart, len);
        return del;
    }
    auto& curr = at(yStart);
    bool isFullLine = xStart == 0;
    del = curr.erase(xStart, curr.length());
    int actualIdx = yStart;
    if(isFullLine)
        lines.erase(lines.begin()+yStart);
    else
        ++actualIdx;
    for(int line=yStart+1;line<yEnd;++line) {
        del += '\n';
        del += at(actualIdx).get();
        lines.erase(lines.begin()+actualIdx);
    }
    del += '\n';
    if(xEnd > 0) {
        auto& last = at(actualIdx);
        isFullLine = xEnd >= last.length();
        del += last.erase(0, xEnd);
        if(isFullLine) {
            lines.erase(lines.begin()+actualIdx);
            del += '\n';
        }
    }
    return del;
}

std::vector<std::string> MultiLine::removeCurrent() {
    std::vector<std::string> del;
    modified = true;
    int len = cursor.count();
    for(int i=0;i<len;++i) {
        auto& culoc = cursor.at(i);
        if(culoc.x == lines[culoc.y].length() && culoc.y == length()-1) {
            del.push_back("");
            continue;
        }
        if(culoc.x < lines[culoc.y].length()) {
            auto str = lines[culoc.y].erase(culoc.x, 1);
            del.push_back(str);
            continue;
        }
        int y = culoc.y;
        int oldy = y + 1;
        const auto& str = lines[oldy].get();
        auto& line = lines[y];
        line.insert(str.c_str(), culoc.x);
        lines.erase(lines.begin()+oldy);
        del.push_back("\n");
    }
    return del;
}

std::vector<std::string> MultiLine::killLine() {
    std::vector<std::string> del;
    modified = true;
    int len = cursor.count();
    for(int i=0;i<len;++i) {
        const auto& culoc = cursor.at(i);
        auto& line = lines.at(culoc.y);
        if(culoc.x >= line.length()) {
            if(culoc.y == length()-1) {
                del.push_back("");
                continue;
            }
            auto& next = lines.at(culoc.y + 1);
            line.insert(next.get(), culoc.x);
            lines.erase(lines.begin() + culoc.y + 1);
            del.push_back("\n");
        } else {
            auto str = line.erase(culoc.x, line.length()-culoc.x);
            del.push_back(str);
        }
    }
    return del;
}

char MultiLine::charAt(const Pos2d<int>& pos) const {
    if(pos.y >= length())
        return ' ';
    const auto& line = at(pos.y);
    if(pos.x >= line.length())
        return ' ';
    return line.at(pos.x);
}

int MultiLine::totalLinesNeeded() const {
    int end = cursor.at(0).y;
    int len = 0;
    for(int i=startLine;i<=end;++i) {
        len += lines[i].numLinesNeeded(screenDim.x);
    }
    return len;
}

void MultiLine::lineUp() {
    while(totalLinesNeeded() > screenDim.y)
        ++startLine;
}

void MultiLine::lineDown() {
    startLine = std::min(startLine, cursor.at(0).y);
}

void MultiLine::lineEnd() {
    auto screen = buffer2screen(cursor.at(0));
    int relY = screen.y - startLine;
    if(relY < screenDim.y)
        return;
    int diff = relY - screenDim.y + 1;
    startLine += diff;
}

void MultiLine::save() {
    if(!modified) {
        CMBAR_MSG("Nothing to save");
        return;
    }
    if(fileName.empty()) {
        auto& ed = Editor::getInstance();
        fileName = ed.prompt("File to save: ");
    }
    if(fileName.empty()) {
        CMBAR_MSG("Empty file name!");
        return;
    }
    std::ofstream fp;
    fp.open(fileName.c_str());
    ASSERT(fp.is_open(), "Failed to open file '%s'!", fileName.c_str());
    int len = (int)lines.size();
    for(int i=0;i<len;++i) {
        // don't write the final line if it is empty
        if(i == len-1 && lines[i].empty())
            continue;
        fp << lines[i].get() << "\n";
    }
    fp.close();
    fileName = rel2abs(pwd(), fileName);
    dirName = dirname(fileName);
    buffName = basename(fileName);
    modified = false;
    CMBAR_MSG("Wrote to '%s'\n", fileName.c_str());
}

void MultiLine::clear() {
    for(auto& line : lines)
        line.clear();
    lines.clear();
    addLine();
    startLine = 0;
    cursor.reset(this);
    disableRegions();
}

int Line::findFirstNotOf(const std::string& str, int pos) const {
    auto res = line.find_first_not_of(str, pos);
    if(res == std::string::npos)
        return line.length();
    return (int)res;
}

int Line::findLastNotOf(const std::string& str, int pos) const {
    auto res = line.find_last_not_of(str, pos);
    if(res == std::string::npos)
        return 0;
    return (int)res;
}


CmdMsgBar::CmdMsgBar(): MultiLine(), minLoc(0), options() {
    populateKeyMap<PromptKeys>(kcMap, true);
}

void CmdMsgBar::resize(const Pos2d<int>& start, const Pos2d<int>& dim) {
    // no status bar!
    screenStart = start;
    screenDim = dim;
}

///@todo: support for filtering on external vector
void CmdMsgBar::drawBuffer(Editor& ed) {
    // first line is always the cmd prompt!
    int y = drawLine(screenStart.y, lines[0].get(), ed, 0,
                     "cmdmsgbarfg", "cmdmsgbarbg");
    if(!usingOptions())
        return;
    int len = (int)options.size();
    int h = screenStart.y + screenDim.y;
    for(int idx=startLine;y<h&&idx<len;++idx)
        y = drawLine(y, options[idx], ed, idx, "defaultfg", "defaultbg");
}

void CmdMsgBar::insert(const char* str) {
    int len = strlen(str);
    for(int i=0;i<len;++i)
        insert(str[i]);
}

// always insert on the first line!
void CmdMsgBar::insert(char c) {
    auto& culoc = cursor.at(0);
    auto& line = lines[0];
    // if not on the first line, get back to it
    if(culoc.y != 0) {
        culoc.y = 0;
        culoc.x = line.length();
    }
    line.insert(c, culoc.x);
    ++culoc.x;
}

void CmdMsgBar::clear() {
    auto& culoc = cursor.at(0);
    auto& line = lines[culoc.y];
    line.erase(0, line.length());
    culoc = {0, 0};
    lineReset();
}


const int Cursor::Hidden = 0xFFFFFFFF;

Cursor::Cursor(): locs() {
    locs.push_back(Pos2d<int>(Hidden, Hidden));
}

Positions Cursor::saveExcursion() const {
    Positions out(locs.size());
    std::copy(locs.begin(), locs.end(), out.begin());
    return out;
}

void Cursor::restoreExcursion(const Positions& pos) {
    locs.clear();
    locs.resize(pos.size());
    std::copy(pos.begin(), pos.end(), locs.begin());
}

void Cursor::home(MultiLine* ml) {
    for(auto& cu : locs) {
        cu.x = ml->getMinStartLoc();
    }
    removeDuplicates();
}

void Cursor::lineEnd(MultiLine* ml) {
    for(auto& cu : locs) {
        cu.x = ml->at(cu.y).length();
    }
    removeDuplicates();
}

void Cursor::left(MultiLine* ml) {
    int minLoc = ml->getMinStartLoc();
    for(auto& cu : locs) {
        --cu.x;
        if(cu.x < minLoc) {
            if(cu.y >= 1) {
                --cu.y;
                cu.x = ml->at(cu.y).length();
            } else {
                cu.y = 0;
                cu.x = minLoc;
            }
        }
    }
    ml->lineDown();
    removeDuplicates();
}

void Cursor::right(MultiLine* ml) {
    for(auto& cu : locs) {
        ++cu.x;
        if(cu.x > ml->at(cu.y).length()) {
            if(cu.y < ml->length()-1) {
                ++cu.y;
                cu.x = 0;
            } else {
                --cu.x;
            }
        }
    }
    ml->lineUp();
    removeDuplicates();
}

void Cursor::down(MultiLine* ml) {
    for(auto& cu : locs) {
        if(cu.y < ml->length()-1) {
            ++cu.y;
            cu.x = std::min(cu.x, ml->at(cu.y).length());
        }
    }
    ml->lineUp();
    removeDuplicates();
}

void Cursor::up(MultiLine* ml) {
    for(auto& cu : locs) {
        if(cu.y >= 1) {
            --cu.y;
            cu.x = std::min(cu.x, ml->at(cu.y).length());
        }
    }
    ml->lineDown();
    removeDuplicates();
}

void Cursor::reset(MultiLine* ml) {
    for(auto& cu : locs) {
        cu.x = 0;
        cu.y = 0;
    }
    ml->lineReset();
    removeDuplicates();
}

void Cursor::end(MultiLine* ml) {
    for(auto& cu : locs) {
        cu.y = std::max(0, ml->length() - 1);
        cu.x = ml->at(cu.y).length();
    }
    ml->lineEnd();
    removeDuplicates();
}

void Cursor::pageDown(MultiLine* ml, float jump) {
    int ijump = ml->verticalJump(jump);
    for(auto& cu : locs) {
        cu.y = std::min(ml->length()-1, cu.y+ijump);
        cu.x = 0;
    }
    ml->lineUp();
    removeDuplicates();
}

void Cursor::pageUp(MultiLine* ml, float jump) {
    int ijump = ml->verticalJump(jump);
    for(auto& cu : locs) {
        cu.y = std::max(0, cu.y-ijump);
        cu.x = 0;
    }
    ml->lineDown();
    removeDuplicates();
}

void Cursor::nextPara(MultiLine* ml) {
    int len = ml->length();
    for(auto& cu : locs) {
        int prevLen = ml->at(cu.y).length();
        for(++cu.y;cu.y<len;++cu.y) {
            if(ml->at(cu.y).length() == 0 && prevLen != 0)
                break;
            prevLen = ml->at(cu.y).length();
        }
        cu.y = std::min(cu.y, len-1);
        cu.x = 0;
    }
    ml->lineUp();
    removeDuplicates();
}

void Cursor::previousPara(MultiLine* ml) {
    for(auto& cu : locs) {
        int prevLen = ml->at(cu.y).length();
        for(--cu.y;cu.y>=0;--cu.y) {
            if(ml->at(cu.y).length() == 0 && prevLen != 0)
                break;
            prevLen = ml->at(cu.y).length();
        }
        cu.y = std::max(cu.y, 0);
        cu.x = 0;
    }
    ml->lineDown();
    removeDuplicates();
}

void Cursor::nextWord(MultiLine* ml) {
    const auto& word = ml->getWord();
    for(auto& cu : locs) {
        const auto& line = ml->at(cu.y);
        if(cu.x >= line.length()) {
            if(cu.y >= ml->length()-1)
                continue;
            ++cu.y;
            cu.x = 0;
        } else {
            cu.x = line.findFirstNotOf(word, cu.x + 1);
        }
    }
    ml->lineUp();
    removeDuplicates();
}

void Cursor::previousWord(MultiLine* ml) {
    const auto& word = ml->getWord();
    for(auto& cu : locs) {
        if(cu.x <= 0) {
            if(cu.y <= 0)
                continue;
            --cu.y;
            cu.x = ml->at(cu.y).length();
        } else {
            const auto& line = ml->at(cu.y);
            cu.x = line.findLastNotOf(word, cu.x - 1);
        }
    }
    ml->lineDown();
    removeDuplicates();
}

bool Cursor::findCursor(const Pos2d<int>& pos) const {
    for(auto& cu : locs) {
        if(cu == pos)
            return true;
    }
    return false;
}

void Cursor::addBack(int cx, int cy) {
    Pos2d<int> pos(cx, cy);
    addBack(pos);
}

void Cursor::addBack(Pos2d<int>& pos) {
    if(!findCursor(pos))
        locs.push_back(pos);
}

void Cursor::addFront(int cx, int cy) {
    Pos2d<int> pos(cx, cy);
    addFront(pos);
}

void Cursor::addFront(Pos2d<int>& pos) {
    if(!findCursor(pos))
        locs.insert(locs.begin(), pos);
}

void Cursor::remove(int id) {
    if(count() >= 2)
        locs.erase(locs.begin()+id);
}

void Cursor::clearAllButFirst() {
    if(count() == 1)
        return;
    auto first = locs[0];
    locs.clear();
    locs.push_back(first);
}

bool Cursor::isHidden(int id) const {
    const Pos2d<int>& p = locs[id];
    return ((p.x == Hidden) || (p.y == Hidden));
}

bool Cursor::isHidden(int cx, int cy) const {
    return ((cx == Hidden) || (cy == Hidden));
}

void Cursor::removeDuplicates() {
    for(int i=0;i<count();++i) {
        for(int j=i+1;j<count();++j) {
            if(locs[i] == locs[j]) {
                locs.erase(locs.begin()+j);
                --j;
            }
        }
    }
}

void Cursor::moveRightCursorsOnSameLine(int i) {
    int len = count();
    const auto& culoc = at(i);
    for(int j=i;j<len;++j) {
        auto& cu = at(j);
        if(cu.y == culoc.y)
            ++cu.x;
    }
}

void Cursor::moveDownAllNextCursors(int i) {
    int len = count();
    for(int j=i;j<len;++j) {
        auto& cu = at(j);
        ++cu.y;
    }
}

void Cursor::moveUpAllNextCursors(int i) {
    int len = count();
    for(int j=i;j<len;++j) {
        auto& cu = at(j);
        --cu.y;
    }
}

void Cursor::moveLeftCursorsOnSameLine(int i) {
    int len = count();
    const auto& culoc = at(i);
    for(int j=i;j<len;++j) {
        auto& cu = at(j);
        if(cu.y == culoc.y)
            --cu.x;
    }
}

} // end namespace teditor
