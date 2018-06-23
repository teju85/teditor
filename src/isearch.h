#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "cmd_msg_bar.h"


namespace teditor {

class Buffer;

/**
 * @brief Incremental search support used by Ctrl-F command
 */
class ISearch: public Choices {
public:
    ISearch(const Buffer& mlb);

    const std::string& at(int idx) const { return ml.at(idx).get(); }
    int size() const { return (int)matches.size(); }
    bool updateChoices(const std::string& str);
    std::string getFinalStr(int idx, const std::string& str) const;
    bool match(int idx, const std::string& str) const { return emptyAt(idx); }

    /** reset the search state */
    void reset();

    /** check whether there are matches at a given line num */
    bool emptyAt(int i) const { return matches.find(i) == matches.end(); }

    /** fetch matches for a given line num */
    const std::vector<int>& matchesAt(int i) const;

private:
    /** buffer where to conduct searches */
    const Buffer& ml;
    /** current search string */
    std::string curr;
    /** line number v/s list of matches */
    std::unordered_map<int, std::vector<int> > matches;

    void searchBuffer();
    void searchLine(const std::string& str, std::vector<int>& res);
};

} // end namespace teditor
