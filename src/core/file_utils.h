#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "utils.h"


namespace teditor {

bool isDir(const std::string& f);
bool isFile(const std::string& f);
bool isReadOnly(const char* f);
void makeDir(const std::string& d);

/** container to hold remote file/dir info */
struct Remote {
  std::string host, file;

  Remote(const std::string& f);
};

bool isRemote(const std::string& f);

std::string slurp(const std::string& file);
Strings slurpToArr(const std::string& file);

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


struct FilePerm {
  FilePerm(const std::string& n);

  std::string name;
  char perms[10];
  size_t size;

  static const int DirModeFileOffset;
};

typedef std::vector<FilePerm> Files;

Files listDir(const std::string& dir);
Strings listDirRel(const std::string& dir);
std::string listDir2str(const std::string& dir);

std::string copyFromRemote(const std::string& file);
void copyToRemote(const std::string& rfile, const std::string& local);
void copyFile(const std::string& in, const std::string& out);

typedef std::pair<std::string, int> FileInfo;

FileInfo readFileInfo(const std::string& arg);

/** Filter function while working with files */
bool fileStrFind(const std::string& line, const std::string& str);

/** check if the file represents current dir or parent dir */
bool isCurrentOrParentDir(const std::string& dir);


/** Cache the outputs of `listDirRel` for faster future access */
class DirCache {
 public:
  /** method to return reference to the underlying singleton object */
  static DirCache& getInstance();

  /**
   * @defgroup CacheUpdate Update ops
   * @{
   * @brief forcefully update cache at the given location 
   */
  static void forceUpdateAt(const std::string& dir, const Files& fs);
  static void forceUpdateAt(const std::string& dir, const std::string& res);
  /** @} */

  /** get the contents of dir */
  static Strings& getDirContents(const std::string& dir);

 private:
  std::unordered_map<std::string, Strings> cache;

  DirCache(): cache() {}

  std::string noTrailingSlash(const std::string& dir);
};  // class DirCache

} // end namespace teditor
