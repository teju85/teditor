#include "isearch.h"
#include "buffer.h"
#include "logger.h"
#include "cmd_msg_bar.h"


namespace teditor {

bool strFindEmpty(const std::string& line, const std::string& str) {
    if(str.empty())
        return true;
    return strFind(line, str);
}


ISearch::ISearch(Buffer& mlb): Choices(strFindEmpty), ml(mlb), curr(),
                               matches() {
}

std::string ISearch::getFinalStr(int idx, const std::string& str) const {
    return at(idx);
}

const std::string& ISearch::at(int idx) const {
    return ml.at(idx).get();
}

const std::vector<int>& ISearch::matchesAt(int i) const {
    const auto itr = matches.find(i);
    ASSERT(itr != matches.end(), "No matches at line '%d'!", i);
    return itr->second;
}

bool ISearch::updateChoices(const std::string& str) {
    if(str == curr)
        return false;
    reset();
    curr = str;
    if(!curr.empty())
        searchBuffer();
    return true;
}

void ISearch::reset() {
    curr.clear();
    matches.clear();
}

void ISearch::searchBuffer() {
    int len = ml.length();
    for(int i=0;i<len;++i) {
        const auto& str = ml.at(i).get();
        std::vector<int> res;
        searchLine(str, res);
        if(!res.empty())
            matches[i] = res;
    }
}

void ISearch::updateMainBuffer(CmdMsgBar& cmBar) {
    int loc = cmBar.getOptLoc();
    ml.gotoLine(loc);
}

void ISearch::searchLine(const std::string& str, std::vector<int>& res) {
    size_t len = str.size();
    size_t loc = 0;
    while(loc < len) {
        loc = str.find(curr, loc);
        if(loc == std::string::npos)
            break;
        res.push_back((int)loc);
        loc += curr.size();
    }
}

} // end namespace teditor