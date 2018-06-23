#include "isearch.h"
#include "buffer.h"


namespace teditor {

ISearch::ISearch(const Buffer& mlb): Choices(strFind), ml(mlb), curr(),
                                     matches() {
}

std::string ISearch::getFinalStr(int idx, const std::string& str) const {
    return at(idx);
}

const std::vector<int>& ISearch::matchesAt(int i) const {
    const auto itr = matches.find(i);
    ASSERT(itr != matches.end(), "No matches at line '%d'!", i);
    return itr->second;
}

bool ISearch::updateChoices(const std::string& str) {
    if(str == curr)
        return false;
    if(str.size() > curr.size() && !curr.empty() &&
       str.substr(0, curr.size()) == curr) {
        curr = str;
        searchBuffer();
        return true;
    }
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
    bool firstSearch = matches.empty();
    int len = ml.length();
    for(int i=0;i<len;++i) {
        const auto& str = ml.at(i).get();
        auto itr = matches.find(i);
        if(itr != matches.end()) {
            searchLine(str, itr->second);
            if(itr->second.empty())
                matches.erase(itr);
        } else if(itr == matches.end() && firstSearch) {
            std::vector<int> res;
            searchLine(str, res);
            if(!res.empty())
                matches[i] = res;
        }
    }
}

void ISearch::searchLine(const std::string& str, std::vector<int>& res) {
    if(res.empty()) {
        size_t len = str.size();
        size_t loc = 0;
        while(loc < len) {
            loc = str.find(curr, loc);
            if(loc == std::string::npos)
                break;
            res.push_back((int)loc);
            loc += curr.size();
        }
    } else {
        const auto& last = curr.back();
        for(size_t i=0;i<res.size();++i) {
            auto pos = res[i] + curr.size() - 1;
            if(str[pos] != last) {
                res.erase(res.begin() + i);
                --i;
            }
        }
    }
}

} // end namespace teditor
