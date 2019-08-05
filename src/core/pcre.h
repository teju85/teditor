#pragma once

#define PCRE2_CODE_UNIT_WIDTH 8
#include "pcre2.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "utils.h"
#include "pos2d.h"


namespace teditor {

/** Structute to represent match location */
typedef Pos2d<int> MatchLoc;

class Pcre;

/**
 * @brief Structure to store match information out of regex match
 */
class Match {
public:
  Match(const std::string& s);

  /** total number of matches found */
  int count() const { return groups; }

  /** check whether it is an empty match */
  bool empty() const { return count() == PCRE2_ERROR_NOMATCH; }

  /** returns the i-th numbered match */
  std::string get(int i) const;

  /** returns the named match */
  std::string get(const std::string& name) const;

  /** returns the i-th numbered match location */
  const MatchLoc& getLoc(int i) const;

  /** returns the named match location */
  const MatchLoc& getLoc(const std::string& name) const;

private:
  /** total number of matches */
  int groups;
  /** input string */
  std::string str;
  /** numbered matches */
  std::vector<MatchLoc> matches;
  /** named matches */
  std::unordered_map<std::string,MatchLoc> namedMatches;

  friend class Pcre;
};


/** vector of matches for 'findAll' function */
typedef std::vector<Match> Matches;


/** Wrapper around the C pcre2 library */
class Pcre {
public:
  /**
   * @brief Constructor, which compiles the input regex
   * @param reg PCRE as a string
   * @param case_ whether to do a caseless match
   */
  Pcre(const std::string& reg, bool case_=false);

  /** Destructor, which finally cleans up the compiled regex */
  ~Pcre();

  /**
   * @brief main function to find matches in the input string
   * @param str the input string
   * @param loc from where to start finding
   * @return the match data
   */
  Match find(const std::string& str, int loc=0);

  /**
   * @brief main function to find all matches in the input string
   * @param str the input string
   * @return the vector of match data
   */
  Matches findAll(const std::string& str);

  /** Specialized function to check for string match */
  bool isMatch(const std::string& str);

  /** pcre2 substitute calls */
  int replace(const std::string& in, const std::string& rep, std::string& out);
  int replaceAll(const std::string& in, const std::string& rep,
                 std::string& out);

private:
  /** regex as C-string */
  std::string regex;
  /** caseless match */
  bool caseless;
  /** compiled regex */
  pcre2_code* re;
  /** match data needed for running this regex against input strings */
  pcre2_match_data* matchData;
  /** namecount for named groups */
  uint32_t namecount;
  /** nametable in case of non-zero namecount */
  PCRE2_SPTR nameTable;
  /** size of nameTable */
  uint32_t nameEntrySize;
  /** offset vector */
  PCRE2_SIZE* ovector;
  /** utf8 string match? */
  bool isUtf8;
  /** is crlf the newline? */
  bool crlfIsNewline;

  void storeGroups(Match& m);
  void storeNumberedGroups(Match& m);
  void storeNamedGroups(Match& m);

  int _replace(const std::string& in, const std::string& rep,
               std::string& out, uint32_t options);

  PCRE2_SIZE utf8Jump(char* subject, size_t subLen, PCRE2_SIZE start);

  bool smallOvector(const Match& m) const { return m.groups == 0; }
  bool kAssertion() const { return ovector[0] > ovector[1]; }
};

} // end namespace teditor
