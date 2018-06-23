#include "cmd_msg_bar.h"
#include <string.h>


namespace teditor {

bool strFind(const std::string& line, const std::string& str) {
    return line.find(str) != std::string::npos;
}


bool Choices::match(const std::string& line, const std::string& str) const {
    return filter(line, str);
}

bool Choices::match(int idx, const std::string& str) const {
    return match(at(idx), str);
}


StringChoices::StringChoices(const std::vector<std::string>& arr,
                             ChoicesFilter cf): Choices(cf), options(arr) {
}

std::string StringChoices::getFinalStr(int idx, const std::string& str) const {
    return at(idx);
}


CmdMsgBar::CmdMsgBar(): MultiLine(), minLoc(0), choices(), optLoc(0) {
    populateKeyMap<PromptKeys>(kcMap, true);
}

void CmdMsgBar::resize(const Pos2d<int>& start, const Pos2d<int>& dim) {
    // no status bar!
    screenStart = start;
    screenDim = dim;
}

void CmdMsgBar::drawBuffer(Editor& ed) {
    // first line is always the cmd prompt!
    int y = drawLine(screenStart.y, lines[0].get(), ed, 0, "cmbarfg", "cmbarbg");
    if(!usingChoices())
        return;
    int len = choices->size();
    int h = screenStart.y + screenDim.y;
    const auto str = getStr();
    for(int idx=startLine;y<h&&idx<len;++idx) {
        const auto& line = choices->at(idx);
        if(!choices->match(line, str))
            continue;
        const char* fg = (idx == optLoc)? "cmbarhighlightfg" : "cmbarfg";
        const char* bg = (idx == optLoc)? "cmbarhighlightbg" : "cmbarbg";
        y = drawLine(y, line, ed, idx, fg, bg);
    }
}

std::string CmdMsgBar::getFinalChoice() const {
    if(usingChoices())
        return choices->getFinalStr(optLoc, getStr());
    return getStr();
}

int CmdMsgBar::linesNeeded(const std::string& str, int wid) const {
    int len = (int)str.size();
    if(len <= 0)
        return 1;
    return (len + wid - 1) / wid;
}

int CmdMsgBar::totalLinesNeeded() const {
    int count = MultiLine::totalLinesNeeded();
    if(!usingChoices())
        return count;
    const auto str = getStr();
    for(int i=startLine;i<=optLoc;++i)
        count += linesNeeded(choices->at(i), screenDim.x);
    return count;
}

void CmdMsgBar::insert(const char* str) {
    auto& culoc = cursor.at(0);
    lines[0].insert(str, culoc.x);
    culoc.x += strlen(str);
}

// always insert on the first line!
void CmdMsgBar::insert(char c) {
    auto& culoc = cursor.at(0);
    lines[0].insert(c, culoc.x);
    ++culoc.x;
    if(!usingChoices())
        return;
    updateChoices();
    // then jump to the first matching option at this point!
    int len = choices->size();
    const auto str = getStr();
    for(int idx=startLine;idx<len;++idx) {
        if(choices->match(idx, str)) {
            optLoc = idx;
            startLine = idx;
            break;
        }
    }
}

void CmdMsgBar::updateChoices() {
    if(!usingChoices())
        return;
    if(choices->updateChoices(getStr()))
        optLoc = 0;
}

void CmdMsgBar::clear() {
    auto& culoc = cursor.at(0);
    auto& line = lines[culoc.y];
    line.erase(0, line.length());
    culoc = {0, 0};
    lineReset();
}

void CmdMsgBar::clearChoices() {
    choices = nullptr;
    optLoc = 0;
}

void CmdMsgBar::down() {
    if(!usingChoices())
        return;
    int len = choices->size();
    const auto str = getStr();
    for(int idx=optLoc+1;idx<len;++idx) {
        if(choices->match(idx, str)) {
            optLoc = idx;
            break;
        }
    }
    lineUp();
}

void CmdMsgBar::up() {
    if(!usingChoices())
        return;
    const auto str = getStr();
    for(int idx=optLoc-1;idx>=0;--idx) {
        if(choices->match(idx, str)) {
            optLoc = idx;
            break;
        }
    }
    lineDown();
}

} // end namespace teditor

