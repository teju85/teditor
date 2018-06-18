#include "pcre.h"
#include "utils.h"
#include "logger.h"


// Courtesy: Code here is straight out of pcre2demo.c!!

///@todo: support UTF8


namespace teditor {

Match::Match(const std::string& s): groups(PCRE2_ERROR_NOMATCH), str(s),
                                    matches(), namedMatches() {
}

const MatchLoc& Match::getLoc(int i) const {
    ASSERT((i >= 0) && (i < groups),
           "Match::get Invalid index '%d'! [0, %d)", i, groups);
    return matches[i];
}

const MatchLoc& Match::getLoc(const std::string& name) const {
    auto itr = namedMatches.find(name);
    ASSERT(itr != namedMatches.end(), "Match::get Bad name passed! '%s'",
           name.c_str());
    return itr->second;
}

std::string Match::get(int i) const {
    const auto& loc = getLoc(i);
    return str.substr(loc.x, loc.y);
}

std::string Match::get(const std::string& name) const {
    const auto& loc = getLoc(name);
    return str.substr(loc.x, loc.y);
}


Pcre::Pcre(const std::string& reg, bool case_):
    regex(reg), caseless(case_), re(nullptr), matchData(nullptr), namecount(0U),
    nameTable(nullptr), nameEntrySize(0U), ovector(nullptr), isUtf8(false),
    crlfIsNewline(false) {
    int errNum;
    PCRE2_SIZE errOffset;
    uint32_t options = caseless? PCRE2_CASELESS : 0;
    re = pcre2_compile((PCRE2_SPTR)regex.c_str(), regex.size(), options,
                       &errNum, &errOffset, nullptr);
    if(re == nullptr) {
        PCRE2_UCHAR buf[256];
        pcre2_get_error_message(errNum, buf, sizeof(buf));
        ASSERT(false, "PCRE: compile failed! regex='%s' offset=%d err='%s'",
               regex.c_str(), (int)errOffset, buf);
    }
    matchData = pcre2_match_data_create_from_pattern(re, nullptr);
    pcre2_pattern_info(re, PCRE2_INFO_NAMECOUNT, &namecount);
    if(namecount != 0) {
        pcre2_pattern_info(re, PCRE2_INFO_NAMETABLE, &nameTable);
        pcre2_pattern_info(re, PCRE2_INFO_NAMEENTRYSIZE, &nameEntrySize);
    }
    ovector = pcre2_get_ovector_pointer(matchData);
    uint32_t optionBits = 0U;
    pcre2_pattern_info(re, PCRE2_INFO_ALLOPTIONS, &optionBits);
    isUtf8 = optionBits & PCRE2_UTF;
    uint32_t newline;
    pcre2_pattern_info(re, PCRE2_INFO_NEWLINE, &newline);
    crlfIsNewline = newline == PCRE2_NEWLINE_ANY ||
        newline == PCRE2_NEWLINE_CRLF ||
        newline == PCRE2_NEWLINE_ANYCRLF;
}

Pcre::~Pcre() {
    pcre2_match_data_free(matchData);
    pcre2_code_free(re);
}

bool Pcre::isMatch(const std::string& str) {
    char* subject = (char*)str.c_str();
    int rc = pcre2_match(re, (PCRE2_SPTR)subject, str.size(), 0, 0,
                         matchData, nullptr);
    return rc > 0;
}

void Pcre::storeGroups(Match& m) {
    storeNumberedGroups(m);
    storeNamedGroups(m);
}

void Pcre::storeNumberedGroups(Match& m) {
    for(int i=0;i<m.groups;i++) {
        int start = ovector[2*i];
        int len = ovector[2*i+1] - start;
        MatchLoc ml = {start, len};
        m.matches.push_back(ml);
    }
}

void Pcre::storeNamedGroups(Match& m) {
    if(namecount == 0)
        return;
    auto tabptr = nameTable;
    for(uint32_t i=0;i<namecount;++i,tabptr+=nameEntrySize) {
        int n = (tabptr[0] << 8) | tabptr[1];
        int start = ovector[2*n];
        int len = ovector[2*n+1] - start;
        MatchLoc ml = {start, len};
        std::string name((char*)(tabptr+2));
        m.namedMatches[name] = ml;
    }
}

Match Pcre::find(const std::string& str, int loc) {
    Match m(str);
    char* subject = (char*)str.c_str();
    m.groups = pcre2_match(re, (PCRE2_SPTR)subject, str.size(), loc, 0,
                           matchData, nullptr);
    if(m.groups < 0)
        return m;
    if(m.groups == 0) {
        MESSAGE("ovector was small for re='%s'!\n", regex.c_str());
        return m;
    }
    if(ovector[0] > ovector[1]) {
        MESSAGE("\\K was used in assertion re='%s' match='%.*s'\n",
                regex.c_str(), (int)(ovector[0] - ovector[1]),
                subject + ovector[1]);
        return m;
    }
    storeGroups(m);
    return m;
}

PCRE2_SIZE Pcre::utf8Jump(char* subject, size_t subLen, PCRE2_SIZE start) {
    if(!isUtf8)
        return start;
    for(;start<subLen;++start) {
        if((subject[start] & 0xc0) != 0x80)
            break;
    }
    return start;
}

Matches Pcre::findAll(const std::string& str) {
    Matches ms;
    Match m(str);
    char* subject = (char*)str.c_str();
    auto subLen = str.size();
    m.groups = pcre2_match(re, (PCRE2_SPTR)subject, str.size(), 0, 0,
                           matchData, nullptr);
    if(m.groups < 0)
        return ms;
    if(m.groups == 0) {
        MESSAGE("ovector was small for re='%s'!\n", regex.c_str());
        return ms;
    }
    if(ovector[0] > ovector[1]) {
        MESSAGE("\\K was used in assertion re='%s' match='%.*s'\n",
                regex.c_str(), (int)(ovector[0] - ovector[1]),
                subject + ovector[1]);
        return ms;
    }
    storeGroups(m);
    ms.push_back(m);
    while(true) {
        Match m(str);
        uint32_t options = 0;
        PCRE2_SIZE startOffset = ovector[1];
        // previous was an empty match?
        if(ovector[0] == ovector[1]) {
            if(ovector[0] >= subLen)
                break;
            options = PCRE2_NOTEMPTY_ATSTART | PCRE2_ANCHORED;
        } else {
            PCRE2_SIZE startchar = pcre2_get_startchar(matchData);
            if(startOffset <= startchar) {
                if(startchar >= subLen)
                    break;
                startOffset = utf8Jump(subject, subLen, startchar+1);
            }
        }
        m.groups = pcre2_match(re, (PCRE2_SPTR)subject, str.size(), startOffset,
                               options, matchData, nullptr);
        // no-match might now mean we have to move to next char!
        if(m.groups == PCRE2_ERROR_NOMATCH) {
            // all matches found
            if(options == 0)
                break;
            ovector[1] = startOffset + 1;
            // advance by one more if CRLF is enabled
            if(crlfIsNewline && startOffset < subLen-1 &&
               subject[startOffset] == '\r' && subject[startOffset+1] == '\n') {
                ovector[1] += 1;
            } else {
                ovector[1] = utf8Jump(subject, subLen, ovector[1]);
            }
            continue;
        }
        // all other errors here means stop further processing!
        if(m.groups < 0)
            break;
        storeGroups(m);
        ms.push_back(m);
    }
    return ms;
}

} // end namespace teditor
