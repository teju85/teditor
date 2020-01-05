#pragma once

#include <algorithm>
#include <string>
#include <stdexcept>
#include <time.h>
#include <vector>
#include <stdarg.h>


namespace teditor {

/** helper function to convert formatted C-strings to std::string */
std::string format(const char* fmt, ...);
std::string format(const char* fmt, va_list& vl);


/**
 * @def THROW(fmat, ...)
 * @brief Macro to throw runtime_error exception
 * @param fmt C-string format containing error message
 */
#define THROW(fmt, ...)                                                 \
  do {                                                                  \
    std::string msg = format("Exception occured! file=%s line=%d: "     \
                             fmt, __FILE__, __LINE__, ##__VA_ARGS__);   \
    throw std::runtime_error(msg);                                      \
  } while(0)

/**
 * @def ASSERT(check, fmt, ...)
 * @brief Macro to assert with runtime_error exception if the check fails
 * @param check the boolean check to be performed
 * @param fmt C-string format containing error message
 */
#define ASSERT(check, fmt, ...)                 \
  do {                                          \
    if(!(check))  THROW(fmt, ##__VA_ARGS__);    \
  } while(0)


/** short-form for vector of strings */
typedef std::vector<std::string> Strings;

/** whether all the strings internally are empty or not */
inline bool allStringsEmpty(const Strings& strs) {
  return std::all_of(strs.begin(), strs.end(),
                     [](const std::string& s) { return s.empty(); });
}

/** whether the 2 vectors are same */
template <typename T>
bool same(const std::vector<T>& s1, const std::vector<T>& s2) {
  if(s1.size() != s2.size()) return false;
  return std::equal(s1.begin(), s1.end(), s2.begin());
}


bool operator>(const struct timeval& ta, const struct timeval& tb);

std::string env(const std::string& s);
std::string expandEnvVars(const std::string& str);

bool startsWith(const char* s1, size_t len, const char* s2);
Strings split(const std::string &s, char delim);
std::string join(const Strings& s, char delim, size_t start = 0, size_t end = 0);

std::string num2str(int num);
int str2num(const std::string& str);
float str2real(const std::string& str);
double str2double(const std::string& str);

/** checks if the given dir is part of a git repo */
bool isUnderGit(const std::string& dir);
/** returns the current branch name of the dir */
std::string gitBranchName(const std::string& dir);


struct CmdStatus {
  std::string output, error;
  int status;
};

CmdStatus check_output(const std::string& cmd);
/** run the given command on the remote host and return its output */
CmdStatus check_output(const std::string& cmd, const std::string& host);

/** @brief Hexify the input url */
std::string urlHexify(const std::string& url);

/** @brief Download the content of input url into the given file */
void downloadUrl(const std::string& url, const std::string& file,
                 const std::string& dnldProg = "curl",
                 const std::string& dnldProgOpts = "-s");

/** @brief Download the content of input url into a string */
std::string downloadUrlToString(const std::string& url,
                                const std::string& dnldProg = "curl",
                                const std::string& dnldProgOpts = "-s");

bool isOpenParen(char c);
bool isCloseParen(char c);
bool isParen(char);
char getMatchingParen(char c);

/** Filter function that just performs std::string::find */
bool strFind(const std::string& line, const std::string& str);
/** Filter function that performs case insensitive find */
bool iStrFind(const std::string& line, const std::string& str);

void dos2unix(std::string& in);
std::string dos2unix(const std::string& in);

} // end namespace teditor
