#pragma once

#include <string>
#include <vector>
#include "utils.h"


namespace teditor {

bool isDir(const char* f);
bool isFile(const char* f);
bool isReadOnly(const char* f);
bool dirExists(const std::string& f);
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
};

typedef std::vector<FilePerm> Files;

Files listDir(const std::string& dir);
Strings listDirRel(const std::string& dir);
std::string listRemoteDir(const std::string& dir);

void copyFile(const std::string& in, const std::string& out);

typedef std::pair<std::string, int> FileInfo;

FileInfo readFileInfo(const std::string& arg);

/** Filter function while working with files */
bool fileStrFind(const std::string& line, const std::string& str);

/** check if the file represents current dir or parent dir */
bool isCurrentOrParentDir(const std::string& dir);

} // end namespace teditor
