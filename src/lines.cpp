#include "lines.h"


namespace teditor {

Lines::Lines(): std::vector<Line>(), capture(true), undoStack(), redoStack() {
    // always start with something!
    push_back(Line());
}

bool Lines::undo() {
    if(undoStack.empty()) return false;
    ///@todo: implement!
    return true;
}

bool Lines::redo() {
    if(redoStack.empty()) return false;
    ///@todo: implement!
    return true;
}

void Lines::insert(const Pos2di& pos, char c) {
    insert(pos, std::string(&c, 1));
}

void Lines::insert(const Pos2di& pos, const std::string& c) {
    OpData op(pos, c);
    op.after = applyInsertOp(op);
    if(capture) {
        undoStack.push(op);
        clearStack(redoStack);
    }
}

void Lines::applyOp(const OpData& op) {
    if(op.type == OpInsert)
        applyInsertOp(op);
    else if(op.type == OpDelete)
        applyDeleteOp(op);
    else
        ASSERT(false, "Lines::applyOp: Incorrect op '%d'!", op.type);
}

Pos2di Lines::applyInsertOp(const OpData& op) {
    Pos2di pos = op.before;
    for(const auto& c : op.chars) {
        if(c == '\n') {
            std::vector<Line>::insert(begin()+pos.y+1, Line());
            auto& oldline = at(pos.y);
            bool eol = pos.x >= oldline.length();
            if(!eol) {
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

void Lines::applyDeleteOp(const OpData& op) {
    ///@todo: implement!
}

void Lines::clearStack(OpStack& st) {
    while(!st.empty())
        st.pop();
}

} // end namespace teditor
