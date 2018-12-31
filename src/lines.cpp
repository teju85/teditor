#include "lines.h"
#include <algorithm>


namespace teditor {

Lines::Lines(): std::vector<Line>(), capture(true), undoStack(), redoStack() {
    // always start with something!
    push_back(Line());
}

bool Lines::undo() {
    if(undoStack.empty()) return false;
    auto top = undoStack.top();
    undoStack.pop();
    redoStack.push(top);
    if(top.type == OpInsert) {
        applyDeleteOp(top);
    } else if(top.type == OpDelete) {
        applyInsertOp(top);
    }
    return true;
}

bool Lines::redo() {
    if(redoStack.empty()) return false;
    auto top = redoStack.top();
    redoStack.pop();
    undoStack.push(top);
    if(top.type == OpInsert) {
        applyInsertOp(top);
    } else if(top.type == OpDelete) {
        applyDeleteOp(top);
    }
    return true;
}

void Lines::insert(const Pos2di& pos, char c) {
    insert(pos, std::string(&c, 1));
}

void Lines::insert(const Pos2di& pos, const std::string& c) {
    OpData op;
    op.before = pos;
    op.chars = c;
    op.type = OpInsert;
    op.after = applyInsertOp(op);
    if(capture) {
        undoStack.push(op);
        clearStack(redoStack);
    }
}

void Lines::remove(const Pos2di& pos) {
    remove(pos, pos);
}

void Lines::remove(const Pos2di& start, const Pos2di& end) {
    Pos2di small, big;
    start.find(small, big, end);
    OpData op;
    op.before = small;
    op.after = big;
    op.type = OpDelete;
    op.chars = applyDeleteOp(op);
    if(capture) {
        undoStack.push(op);
        clearStack(redoStack);
    }
}

Pos2di Lines::applyInsertOp(const OpData& op) {
    Pos2di pos = op.before;
    for(const auto& c : op.chars) {
        if(c == '\n') {
            std::vector<Line>::insert(begin()+pos.y+1, Line());
            auto& oldline = at(pos.y);
            if(pos.x < oldline.length()) {
                auto res = oldline.erase(pos.x, oldline.length()-pos.x);
                auto& newline = at(pos.y + 1);
                newline.append(res.c_str());
            }
            ++pos.y;
            pos.x = 0;
        } else {
            auto& line = at(pos.y);
            line.insert(c, pos.x);
            ++pos.x;
        }
    }
    return pos;
}

std::string Lines::applyDeleteOp(const OpData& op) {
    std::string del;
    Pos2di start = op.before;
    Pos2di end = op.after;
    while(end > start) {
        auto& curr = at(end.y);
        if(end.x == 0) { // first char in line
            del += curr.erase(end.x, 1);
            // if not the first line itself!
            if(end.y > 0) {
                --end.y;
                end.x = at(end.y).length();
            }
        } else if(end.x == curr.length()) { // last char in line
            // if not the last line itself!
            if(end.y < (int)size() - 1) {
                auto& prev = at(end.y + 1);
                curr.join(prev);
                erase(begin() + end.y + 1);
                del += '\n';
            }
            --end.x;
        } else {
            auto& curr = at(end.y);
            del += curr.erase(end.x, 1);
            --end.x;
        }
    }
    // the starting char would have been left out in the loop
    auto& curr = at(end.y);
    del += curr.erase(end.x, 1);
    // deleted chars would have been reversed!
    std::reverse(del.begin(), del.end());
    return del;
}

void Lines::clearStack(OpStack& st) {
    while(!st.empty())
        st.pop();
}

} // end namespace teditor
