#pragma once

#include <algorithm>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <time.h>
#include <vector>
#include <set>
#include <stdarg.h>
#include <initializer_list>


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
  for(const auto& str : strs)
    if(!str.empty()) return false;
  return true;
}

/** whether the 2 vectors are same */
template <typename T>
bool same(const std::vector<T>& s1, const std::vector<T>& s2) {
  if(s1.size() != s2.size()) return false;
  return std::equal(s1.begin(), s1.end(), s2.begin());
}


bool operator>(const struct timeval& ta, const struct timeval& tb);

bool isDir(const char* f);
bool isFile(const char* f);
bool isReadOnly(const char* f);
bool dirExists(const std::string& f);
void makeDir(const std::string& d);

bool isRemote(const char* f);

std::string env(const char* s);
std::string env(const std::string& s);
std::string expandEnvVars(const std::string& str, const Strings& vars);

std::string slurp(const char* file);
std::string slurp(const std::string& file);
Strings slurpToArr(const char* file);
Strings slurpToArr(const std::string& file);

bool startsWith(const char* s1, int len, const char* s2);
Strings split(const std::string &s, char delim);

std::string num2str(int num);
int str2num(const std::string& str);
float str2real(const std::string& str);
double str2double(const std::string& str);

std::string getpwd();
bool isAbs(const std::string& file);
std::string rel2abs(const std::string& pwd, const std::string& rel);
std::string basename(const std::string& file);
std::string dirname(const std::string& file);
std::string tempFileName();
/** Tries to find the file starting from dir all the way uptill root */
std::string findFirstUpwards(const std::string& dir, const std::string& file);
/** get file's extension from its name */
std::string extension(const std::string& name);
/** checks if the given dir is part of a git repo */
bool isUnderGit(const std::string& dir);
/** returns the current branch name of the dir */
std::string gitBranchName(const std::string& dir);


struct CmdStatus {
  std::string output;
  int status;
};

CmdStatus check_output(const std::string& cmd);


struct FilePerm {
  FilePerm(const std::string& n);

  std::string name;
  char perms[10];
  size_t size;
};

typedef std::vector<FilePerm> Files;

Files listDir(const std::string& dir);
Strings listDirRel(const std::string& dir);

void copyFile(const std::string& in, const std::string& out);

/** @brief Hexify the input url */
std::string urlHexify(const std::string& url);

/** @brief Download the content of input url into the given file */
void downloadUrl(const std::string& url, const std::string& file);

/** @brief Download the content of input url into a string */
std::string downloadUrlToString(const std::string& url);

bool isOpenParen(char c);
bool isCloseParen(char c);
bool isParen(char);
char getMatchingParen(char c);

typedef std::pair<std::string, int> FileInfo;

FileInfo readFileInfo(const std::string& arg);

/** Filter function that just performs std::string::find */
bool strFind(const std::string& line, const std::string& str);

/** Filter function while working with files */
bool fileStrFind(const std::string& line, const std::string& str);

/** check if the file represents current dir or parent dir */
bool isCurrentOrParentDir(const std::string& dir);

} // end namespace teditor
