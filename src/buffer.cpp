#include "buffer.h"
#include <cstring>
#include <unistd.h>
#include "editor.h"
#include "logger.h"
#include <iostream>
#include <fstream>
#include "gtest/gtest_prod.h"
#include <unistd.h>
#include <algorithm>
#include <stack>


namespace teditor {

Buffer::Buffer(const std::string& name):
    screenStart(), screenDim(), lines(), startLine(0), cursor(),
    modified(false), readOnly(false), buffName(name), fileName(), dirName(),
    gitBranch(), regions(), regionActive(false), cmds(), topCmd(-1), mode() {
    addLine();
    cursor.reset(this);
    dirName = getpwd();
    textMode(mode);
}

Buffer::~Buffer() {
    if(!fileName.empty() && Editor::instanceReady()) {
        auto& ed = Editor::getInstance();
        int line = cursor.at(0).y;
        ed.addFileHistory(fileName, line);
    }
}

const AttrColor& Buffer::getColor(const std::string& name) const {
    return mode.cMap.get(name);
}

void Buffer::addCommand(CmdPtr c) {
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

void Buffer::undo() {
    if(topCmd < 0) {
        CMBAR_MSG("No further undo!");
        return;
    }
    cmds[topCmd]->exec(CMD_UNDO);
    --topCmd;
}

void Buffer::redo() {
    int len = (int)cmds.size();
    if(topCmd >= len-1) {
        CMBAR_MSG("No further redo!");
        return;
    }
    topCmd++;
    cmds[topCmd]->exec(CMD_REDO);
}

Strings Buffer::regionAsStr() const {
    Strings out;
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

void Buffer::enableRegions() {
    regionActive = true;
    regions.enable(cursor.getLocs());
}

void Buffer::disableRegions() {
    regionActive = false;
    regions.clear();
}

void Buffer::resize(const Pos2d<int>& start, const Pos2d<int>& dim) {
    screenStart = start;
    screenDim = dim;
    // last line is kept for status bar for this buffer
    --screenDim.y;
    lineUp();
}

void Buffer::load(const std::string& file, int line) {
    if(isDir(file.c_str()))
        loadDir(file);
    else
        loadFile(file, line);
    gitBranch = gitBranchName(dirName);
}

void Buffer::reload() {
    clear();
    load(fileName, 0);
}
void Buffer::loadDir(const std::string& dir) {
    Files fs = listDir(dir);
    auto first = rel2abs(pwd(), dir);
    lines.at(length()-1).append(first.c_str());
    for(const auto& f : fs) {
        auto fname = (f.name == "." || f.name == "..")? f.name : basename(f.name);
        auto buff = format("  %10.10s  %8lu  %s", f.perms, f.size, fname.c_str());
        addLine();
        lines.at(length()-1).append(buff.c_str());
    }
    resetBufferState(0, first);
    cursor.reset(this);
    readOnly = true;
    dirMode(mode);
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
        lines.at(length()-1).append(currLine.c_str());
        addLine();
    }
    fp.close();
    resetBufferState(line, absFile);
    cursor.at(0) = {0, line};
    textMode(mode);
}

void Buffer::resetBufferState(int line, const std::string& file) {
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

void Buffer::drawBuffer(Editor& ed) {
    // draw current buffer
    int h = screenStart.y + screenDim.y;
    int len = length();
    for(int y=screenStart.y,idx=startLine;y<h&&idx<len;++idx)
        y = drawLine(y, lines[idx].get(), ed, idx, "defaultfg", "defaultbg");
}

void Buffer::drawCursor(Editor& ed, const std::string& bg) {
    int n = cursor.count();
    for(int i=0;i<n;++i) {
        auto& culoc = cursor.at(i);
        char c = charAt(culoc);
        auto screenloc = buffer2screen(culoc);
        DEBUG("drawCursor: i=%d x,y=%d,%d sloc=%d,%d start=%d\n",
              i, culoc.x, culoc.y, screenloc.x, screenloc.y, startLine);
        ed.sendChar(screenloc.x, screenloc.y, "cursorfg", bg, c);
    }
}

void Buffer::drawStatusBar(Editor& ed) {
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
        count += ed.sendStringf(x+count, y, "statusfg", "statusbg",
                                " mc:%d", cursor.count());
    }
    // mode
    count += ed.sendStringf(x+count, y, "statusfg", "statusbg",
                            " [mode=%s]", mode.name.c_str());
    // git branch
    if(!gitBranch.empty())
        count += ed.sendStringf(x+count, y, "statusfg", "statusbg",
                                " Git-%s", gitBranch.c_str());
}

std::string Buffer::dirModeGetFileAtLine(int line) {
    if(line == 0)
        return "";
    auto& str = at(line).get();
    return str.substr(dirModeFileOffset());
}

int Buffer::drawLine(int y, const std::string& line, Editor& ed, int lineNum,
                     const std::string& fg, const std::string& bg) {
    int xStart = screenStart.x;
    int wid = screenDim.x;
    int start = 0;
    int len = (int)line.size();
    // empty line
    if(len <= 0)
        return y + 1;
    const auto* str = line.c_str();
    bool isD = mode.name == "dir";
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
            if(regions.isInside(lineNum, start+i, cursor))
                ed.sendChar(xStart+i, y, "highlightfg", "highlightbg", c);
            else if(isD)
                ed.sendChar(xStart+i, y, "dirfg", bg, c);
            else
                ed.sendChar(xStart+i, y, fg, bg, c);
        }
        start += wid;
        ++y;
    }
    return y;
}

Pos2d<int> Buffer::buffer2screen(const Pos2d<int>& loc) const {
    int w = screenDim.x;
    Pos2d<int> ret = screenStart;
    int relY = loc.y - startLine;
    for(int idx=0;idx<relY;++idx)
        ret.y += lines[idx].numLinesNeeded(w);
    ret.y += (loc.x / w);
    ret.x += loc.x % w;
    return ret;
}

Pos2d<int> Buffer::screen2buffer(const Pos2d<int>& loc) const {
    Pos2d<int> rel = {loc.x - screenStart.x, loc.y - screenStart.y};
    Pos2d<int> res = {0, startLine};
    int w = screenDim.x;
    int sy = 0;
    for(;sy<=rel.y;++res.y)
        sy += lines[res.y].numLinesNeeded(w);
    if(sy > rel.y)
        --res.y;
    int dely = rel.y - sy + lines[res.y].numLinesNeeded(w);
    res.x = dely * w + rel.x;
    return res;
}

void Buffer::insertLine(int i) {
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

void Buffer::insert(char c) {
    modified = true;
    int len = cursor.count();
    for(int i=0;i<len;++i) {
        insert(c, i);
    }
    lineUp();
    cursor.removeDuplicates();
}

///@todo: what if a single line crosses the whole screen!?
void Buffer::insert(char c, int i) {
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

void Buffer::insert(const Strings& strs) {
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

void Buffer::insert(const char* buf) {
    int len = strlen(buf);
    for(int i=0;i<len;++i) {
        insert(buf[i]);
    }
}

RemovedLines Buffer::keepRemoveLines(Pcre& pc, bool keep) {
    RemovedLines res;
    bool isRegion = isRegionActive();
    for(int i=0;i<(int)lines.size();++i) {
        if(isRegion && !regions.isInside(i, 0, cursor))
            continue;
        const std::string str = lines[i].get();
        bool match = pc.isMatch(str);
        if((match && keep) || (!match && !keep))
            continue;
        lines.erase(lines.begin()+i);
        res.push_back({str, {0, i}});
        --i;
    }
    if(!res.empty()) {
        cursor.reset(this);
        modified = true;
    }
    // ensure that you don't segfault on full buffer removal!
    if(length() <= 0)
        addLine();
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
    cursor.reset(this);
}

void Buffer::gotoLine(int lineNum) {
    int len = cursor.count();
    for(int i=0;i<len;++i) {
        auto& culoc = cursor.at(i);
        culoc.y = std::min((int)lines.size()-1, std::max(0, lineNum));
    }
    cursor.removeDuplicates();
    startLine = std::max(0, lineNum - screenDim.y/2);
}

void Buffer::matchCurrentParen() {
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

Pos2d<int> Buffer::matchCurrentParen(int i, bool& isOpen) {
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

Strings Buffer::remove() {
    Strings del;
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

Strings Buffer::remove(const Positions& start, const Positions& end) {
    modified = true;
    ASSERT(start.size() == end.size(),
           "remove: start length not same as end's [%lu,%lu]\n",
           start.size(), end.size());
    Strings del;
    int len = (int)start.size();
    for(int i=0;i<len;++i)
        del.push_back(removeFrom(start[i], end[i]));
    return del;
}

std::string Buffer::removeFrom(const Pos2d<int>& start,
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
        del += last.erase(0, xEnd);
    }
    return del;
}

Strings Buffer::removeCurrent() {
    Strings del;
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

Strings Buffer::killLine() {
    Strings del;
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

void Buffer::sortRegions() {
    modified = true;
    int len = cursor.count();
    for(int i=0;i<len;++i) {
        auto& culoc = cursor.at(i);
        int cuy = culoc.y;
        int ry = regions.at(i).y;
        DEBUG("sortRegions: cuy=%d ry=%d i=%d\n", cuy, ry, i);
        sort(lines.begin()+ry, lines.begin()+cuy+1, LineCompare);
        DEBUG("sortRegions: done i=%d\n", i);
        culoc.x = lines[cuy].length();
    }
}

char Buffer::charAt(const Pos2d<int>& pos) const {
    if(pos.y >= length())
        return ' ';
    const auto& line = at(pos.y);
    if(pos.x >= line.length())
        return ' ';
    return line.at(pos.x);
}

int Buffer::totalLinesNeeded() const {
    int end = cursor.at(0).y;
    int len = 0;
    for(int i=startLine;i<=end;++i)
        len += lines[i].numLinesNeeded(screenDim.x);
    return len;
}

void Buffer::lineUp() {
    while(totalLinesNeeded() > screenDim.y)
        ++startLine;
}

void Buffer::lineDown() {
    startLine = std::min(startLine, cursor.at(0).y);
}

void Buffer::lineEnd() {
    auto screen = buffer2screen(cursor.at(0));
    int relY = screen.y - startLine;
    if(relY < screenDim.y)
        return;
    int diff = relY - screenDim.y + 1;
    startLine += diff;
}

void Buffer::save() {
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

void Buffer::clear() {
    for(auto& line : lines)
        line.clear();
    lines.clear();
    addLine();
    startLine = 0;
    cursor.reset(this);
    disableRegions();
}

void Buffer::indent(int line) {
    if(line < length()) {
        int count = mode.indent->indent(*this, line);
        if(count > 0) {
            std::string in(count, ' ');
            lines[line].prepend(in.c_str());
            modified = true;
        }
    }
}

} // end namespace teditor
