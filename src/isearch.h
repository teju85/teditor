#pragma once

#include <string>
#include <vector>
#include <unordered_map>


namespace teditor {

class Buffer;

/**
 * @brief Incremental search support used by Ctrl-F command
 */
class ISearch {
public:
    ISearch(const Buffer& mlb);

    /** add the extra char for the search */
    void addChar(char c);

    /** remove the last char and do a fresh search */
    void removeLast();

    /** reset the search state */
    void reset();

    /** check whether there are matches at a given line num */
    bool emptyAt(int i) const { return matches.find(i) == matches.end(); }

    /** fetch matches for a given line num */
    const std::vector<int>& at(int i) const;

private:
    /** buffer where to conduct searches */
    const Buffer& ml;
    /** current search */
    std::string curr;
    /** line number v/s list of matches */
    std::unordered_map<int, std::vector<int> > matches;

    void searchBuffer();
    void searchLine(const std::string& str, std::vector<int>& res);
};

} // end namespace teditor
