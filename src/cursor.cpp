#include "cursor.h"
#include "buffer.h"


namespace teditor {

Cursor::Cursor(): locs() {
    locs.push_back(Pos2d<int>(0, 0));
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

void Cursor::home(Buffer* ml) {
    int minLoc = ml->getMinStartLoc();
    for(auto& cu : locs)
        cu.x = minLoc;
    removeDuplicates();
}

void Cursor::lineEnd(Buffer* ml) {
    for(auto& cu : locs)
        cu.x = ml->at(cu.y).length();
    removeDuplicates();
}

void Cursor::left(Buffer* ml) {
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

void Cursor::right(Buffer* ml) {
    for(auto& cu : locs) {
        ++cu.x;
        if(cu.x > ml->at(cu.y).length()) {
            if(cu.y < ml->length()-1) {
                ++cu.y;
                cu.x = 0;
            } else
                --cu.x;
        }
    }
    ml->lineUp();
    removeDuplicates();
}

void Cursor::down(Buffer* ml) {
    for(auto& cu : locs) {
        if(cu.y < ml->length()-1) {
            ++cu.y;
            cu.x = std::min(cu.x, ml->at(cu.y).length());
        }
    }
    ml->lineUp();
    removeDuplicates();
}

void Cursor::up(Buffer* ml) {
    for(auto& cu : locs) {
        if(cu.y >= 1) {
            --cu.y;
            cu.x = std::min(cu.x, ml->at(cu.y).length());
        }
    }
    ml->lineDown();
    removeDuplicates();
}

void Cursor::reset(Buffer* ml) {
    for(auto& cu : locs) {
        cu.x = 0;
        cu.y = 0;
    }
    ml->lineReset();
    removeDuplicates();
}

void Cursor::end(Buffer* ml) {
    for(auto& cu : locs) {
        cu.y = std::max(0, ml->length() - 1);
        cu.x = ml->at(cu.y).length();
    }
    ml->lineEnd();
    removeDuplicates();
}

void Cursor::pageDown(Buffer* ml, float jump) {
    int ijump = ml->verticalJump(jump);
    for(auto& cu : locs) {
        cu.y = std::min(ml->length()-1, cu.y+ijump);
        cu.x = 0;
    }
    ml->lineUp();
    removeDuplicates();
}

void Cursor::pageUp(Buffer* ml, float jump) {
    int ijump = ml->verticalJump(jump);
    for(auto& cu : locs) {
        cu.y = std::max(0, cu.y-ijump);
        cu.x = 0;
    }
    ml->lineDown();
    removeDuplicates();
}

void Cursor::nextPara(Buffer* ml) {
    int len = ml->length();
    for(auto& cu : locs) {
        int prevLen = ml->at(cu.y).length();
        for(++cu.y;cu.y<len;++cu.y) {
            if(ml->at(cu.y).length() == 0 && prevLen != 0) break;
            prevLen = ml->at(cu.y).length();
        }
        cu.y = std::min(cu.y, len-1);
        cu.x = 0;
    }
    ml->lineUp();
    removeDuplicates();
}

void Cursor::previousPara(Buffer* ml) {
    for(auto& cu : locs) {
        int prevLen = ml->at(cu.y).length();
        for(--cu.y;cu.y>=0;--cu.y) {
            if(ml->at(cu.y).length() == 0 && prevLen != 0) break;
            prevLen = ml->at(cu.y).length();
        }
        cu.y = std::max(cu.y, 0);
        cu.x = 0;
    }
    ml->lineDown();
    removeDuplicates();
}

void Cursor::nextWord(Buffer* ml) {
    const auto& word = ml->getWord();
    for(auto& cu : locs) {
        const auto& line = ml->at(cu.y);
        if(cu.x >= line.length()) {
            if(cu.y >= ml->length()-1) continue;
            ++cu.y;
            cu.x = 0;
        } else {
            cu.x = line.findFirstNotOf(word, cu.x + 1);
        }
    }
    ml->lineUp();
    removeDuplicates();
}

void Cursor::previousWord(Buffer* ml) {
    const auto& word = ml->getWord();
    for(auto& cu : locs) {
        if(cu.x <= 0) {
            if(cu.y <= 0) continue;
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
    for(auto& cu : locs)
        if(cu == pos) return true;
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
    if(!findCursor(pos)) locs.insert(locs.begin(), pos);
}

void Cursor::remove(int id) {
    if(count() >= 2) locs.erase(locs.begin()+id);
}

void Cursor::clearAllButFirst() {
    if(count() == 1) return;
    auto first = locs[0];
    locs.clear();
    locs.push_back(first);
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
        if(cu.y == culoc.y) --cu.x;
    }
}

bool Cursor::hasCursor(int line) const {
    for(const auto& cu : locs)
        if(line == cu.y) return true;
    return false;
}


void Regions::enable(const Positions& p) {
    clear();
    resize(p.size());
    std::copy(p.begin(), p.end(), begin());
}

bool Regions::isInside(int y, int x, const Cursor& cu, int i) const {
    const auto& p = at(i);
    const auto& culoc = cu.at(i);
    Pos2d<int> start, end;
    p.find(start, end, culoc);
    if(start.y < y && y < end.y) return true;
    else if(start.y == end.y && start.y == y) {
        if(start.x <= x && x <= end.x) return true;
    } else if(start.y == y && x >= start.x) return true;
    else if(end.y == y && x <= end.x) return true;
    return false;
}

bool Regions::isInside(int y, int x, const Cursor& cu) const {
    if(empty()) return false;
    int len = cu.count();
    for(int i=0;i<len;++i)
        if(isInside(y, x, cu, i)) return true;
    return false;
}

} // end namespace teditor
