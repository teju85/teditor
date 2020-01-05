#include <sys/stat.h>
#include "file_utils.h"
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <cctype>
#include <algorithm>
#include "logger.h"

namespace teditor {

std::string remoteFileType(const std::string& f) {
  Remote r(f);
  auto cmd = format("ssh %s /bin/bash -c \\\"file %s\\\"",
                    r.host.c_str(), r.file.c_str());
  auto out = check_output(cmd);
  auto str = out.output;
  // "<file>: " removed
  str.erase(0, r.file.size() + 2);
  return str;
}

bool isRemoteDir(const std::string& type) {
  return type.find("directory") != std::string::npos;
}

bool isDir(const std::string& f) {
  if(!isRemote(f)) {
    struct stat st;
    if(stat(f.c_str(), &st) != 0) return false;
    return S_ISDIR(st.st_mode);
  }
  return isRemoteDir(remoteFileType(f));
}

bool isFile(const std::string& f) {
  if(!isRemote(f)) {
    struct stat st;
    if(stat(f.c_str(), &st) != 0) return false;
    return S_ISREG(st.st_mode);
  }
  return !isRemoteDir(remoteFileType(f));
}

bool isReadOnly(const char* f) {
  return access(f, R_OK) == 0 && access(f, W_OK) < 0;
}

void makeDir(const std::string& d) {
  if (!isRemote(d)) {
    if (isDir(d)) return;
    int ret = mkdir(d.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    ASSERT(ret >= 0, "makeDir: '%s' failed!", d.c_str());
  } else {
    Remote r(d);
    auto cmd = format("ssh %s mkdir -p %s", r.host, r.file);
    auto out = check_output(cmd);
    ASSERT(out.status == 0, "makeDir: cmd='%s' failed! code=%d\n", cmd.c_str(),
           out.status);
  }
}

Remote::Remote(const std::string& f) {
  auto tokens = split(f, ':');
  ASSERT(tokens.size() == 3 && tokens[0] == "/ssh",
         "Remote file syntax must be '%s'. [%s]", "/ssh:<host>:<abspath>",
         f.c_str());
  host = tokens[1];
  file = tokens[2];
}

bool isRemote(const std::string& f) { return !strncmp(f.c_str(), "/ssh:", 5); }

std::string slurp(const std::string& file) {
  FILE *f = fopen(file.c_str(), "rb");
  ASSERT(f, "Failed to open file '%s'!", file.c_str());
  struct stat st;
  bool status = fstat(fileno(f), &st) == 0;
  if(!status) fclose(f);
  ASSERT(status, "Failed to fstat file '%s'!", file.c_str());
  auto len = static_cast<size_t>(st.st_size);
  std::string data(len, '\0');
  status = fread(&(data[0]), 1, st.st_size, f) == len;
  if(!status) fclose(f);
  ASSERT(status, "Failed to fread file '%s'!", file.c_str());
  fclose(f);
  return data;
}

Strings slurpToArr(const std::string& file) {
  Strings ret;
  std::fstream fp;
  fp.open(file.c_str(), std::fstream::in);
  ASSERT(fp.is_open(), "Failed to open file '%s'!", file.c_str());
  std::string currLine;
  while(std::getline(fp, currLine, '\n')) ret.push_back(currLine);
  return ret;
}

std::string getpwd() {
  char cwd[2048];
  (void)getcwd(cwd, 2048);
  return std::string(cwd);
}

bool isAbs(const std::string& file) { return !file.empty() && file[0] == '/'; }

std::string rel2abs(const std::string& pwd, const std::string& rel) {
  char abs[2048];
  auto currCwd = getpwd();
  (void)chdir(pwd.c_str());
  (void)realpath(rel.c_str(), abs);
  (void)chdir(currCwd.c_str());
  std::string res(abs);
  if(res.empty()) res = pwd + '/' + rel;
  return res;
}

std::string basename(const std::string& file) {
  std::string tmp(file);
  if(tmp.back() == '/') tmp.pop_back();
  return tmp.substr(tmp.find_last_of("/\\") + 1);
}

std::string dirname(const std::string& file) {
  auto tmp = file;
  if(file == "/") return "/";
  if(!tmp.empty() && tmp.back() == '/') tmp.pop_back();
  auto loc = tmp.find_last_of("/\\");
  if(loc == std::string::npos) return "";
  return tmp.substr(0, loc);
}

std::string tempFileName() {
  const std::string chars("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_");
  std::string file("/tmp/teditor-remote-");
  for(int i=0;i<16;++i) {
    int idx = rand() % chars.length();
    file.push_back(chars[idx]);
  }
  return file;
}

std::string findFirstUpwards(const std::string& dir, const std::string& file) {
  if(dir.empty() || file.empty()) return "";
  auto d = dir;
  if(d.back() == '/') d.pop_back();
  auto f = d + '/' + file;
  if(isFile(f) || isDir(f)) return f;
  while(!d.empty() && d != "/") {
    d = dirname(d);
    f = d + '/' + file;
    if(isFile(f) || isDir(f)) return f;
  }
  return "";
}

std::string extension(const std::string& name) {
  auto loc = name.find_last_of('.');
  if(loc == std::string::npos) return "";
  return name.substr(loc+1);
}


const size_t FilePerm::DirModeFileOffset = 24;

FilePerm::FilePerm(const std::string& n): name(n), perms(), size(0) {
  struct stat st;
  ASSERT(stat(n.c_str(), &st) == 0, "'stat' failed on '%s'!", n.c_str());
  perms[0] = S_ISDIR(st.st_mode)?  'd' : '-';
  perms[1] = S_IRUSR & st.st_mode? 'r' : '-';
  perms[2] = S_IWUSR & st.st_mode? 'w' : '-';
  perms[3] = S_IXUSR & st.st_mode? 'x' : '-';
  perms[4] = S_IRGRP & st.st_mode? 'r' : '-';
  perms[5] = S_IWGRP & st.st_mode? 'w' : '-';
  perms[6] = S_IXGRP & st.st_mode? 'x' : '-';
  perms[7] = S_IROTH & st.st_mode? 'r' : '-';
  perms[8] = S_IWOTH & st.st_mode? 'w' : '-';
  perms[9] = S_IXOTH & st.st_mode? 'x' : '-';
  size = perms[0] == 'd'? 0 : st.st_size;
}

bool FileCompare(const FilePerm& a, const FilePerm& b) {
  return strcmp(a.name.c_str(), b.name.c_str()) < 0;
}

// From: http://www.gnu.org/software/libc/manual/html_node/Simple-Directory-Lister.html
Files listDir(const std::string& dir) {
  Files f;
  DIR *dp = opendir(dir.c_str());
  if(dp == nullptr) return f;
  for(struct dirent *ep=readdir(dp);ep!=nullptr;ep=readdir(dp)) {
    std::string relFile(ep->d_name);
    std::string file(dir);
    if(file.back() != '/') file += '/';
    file += relFile;
    FilePerm fp(file);
    if(isCurrentOrParentDir(relFile)) fp.name = relFile;
    f.push_back(fp);
  }
  closedir(dp);
  std::sort(f.begin(), f.end(), FileCompare);
  DirCache::forceUpdateAt(dir, f);
  return f;
}

Strings listDirRel(const std::string& dir) {
  if (!isRemote(dir)) {
    Strings f;
    DIR *dp = opendir(dir.c_str());
    if(dp == nullptr) return f;
    for(struct dirent *ep=readdir(dp);ep!=nullptr;ep=readdir(dp))
      f.push_back(ep->d_name);
    closedir(dp);
    std::sort(f.begin(), f.end());
    return f;
  } else {
    Remote r(dir);
    auto cmd = format("ssh %s /bin/bash -c '\"cd %s && ls -a |"
                      " xargs -d \\\"\n\\\" stat --format \\\"%%n\\\"\"'",
                      r.host.c_str(), r.file.c_str());
    auto out = check_output(cmd);
    return split(out.output, '\n');
  }
}

std::string listDir2str(const std::string& dir) {
  std::string cmd;
  if (isRemote(dir)) {
    Remote r(dir);
    cmd = format("ssh %s /bin/bash -c '\"cd %s && ls -a |"
                 " xargs -d \\\"\n\\\" stat --format \\\"  %%A  %%8s  %%n\\\"\"'",
                 r.host.c_str(), r.file.c_str());
  } else {
    cmd = format("cd %s && ls -a | xargs -d '\n' stat --format '  %%A  %%8s  %%n'",
                 dir.c_str());
  }
  auto out = check_output(cmd);
  DirCache::forceUpdateAt(dir, out.output);
  return out.output;
}

void copyFile(const std::string& in, const std::string& out) {
  struct stat st;
  ASSERT(stat(in.c_str(), &st) == 0, "'stat' failed on '%s'!", in.c_str());
  int ifp = open(in.c_str(), O_RDONLY, 0);
  int ofp = open(out.c_str(), O_WRONLY | O_CREAT, st.st_mode);
  size_t size;
  char buf[BUFSIZ];
  while((size = read(ifp, buf, BUFSIZ)) > 0) (void)write(ofp, buf, size);
  close(ifp);
  close(ofp);
}

FileInfo readFileInfo(const std::string& arg) {
  std::string file;
  size_t line;
  const auto tokens = split(arg, ';');
  if(tokens.size() == 1U) {
    file = tokens[0];
    line = 0;
  } else if(tokens.size() == 2U) {
    file = tokens[0];
    line = str2num(tokens[1]);
  } else {
    ASSERT(false, "File: Bad arg passed. Usage: <file>[;<line>]. '%s'!\n",
           arg.c_str());
  }
  if (file.back() == '/') file.erase(file.begin() + file.length() - 1);
  return FileInfo(file, line);
}

bool fileStrFind(const std::string& line, const std::string& str) {
  auto loc = str.find_last_of('/');
  // this shouldn't happen in reality, though!
  if(loc == std::string::npos) return strFind(line, str);
  // pick the basename
  auto sub = str.substr(loc+1);
  if(sub.empty()) return true;
  return strFind(line, sub);
}

bool isCurrentOrParentDir(const std::string& dir) {
  return dir == "." || dir == "..";
}

std::string copyFromRemote(const std::string& file) {
  std::string tmp = tempFileName();
  Remote r(file);
  auto cmd = format("scp %s:%s %s", r.host.c_str(), r.file.c_str(),
                    tmp.c_str());
  check_output(cmd);
  return tmp;
}

void copyToRemote(const std::string& rfile, const std::string& local) {
  Remote r(rfile);
  auto cmd = format("scp %s %s:%s", local.c_str(), r.host.c_str(),
                    r.file.c_str());
  check_output(cmd);
}


DirCache& DirCache::getInstance() {
  static DirCache obj;
  return obj;
}

void DirCache::forceUpdateAt(const std::string& dir, const Files& fs) {
  auto& obj = getInstance();
  auto d = obj.noTrailingSlash(dir);
  Strings files;
  for (const auto& f : fs) files.push_back(f.name);
  obj.cache[d] = files;
}

void DirCache::forceUpdateAt(const std::string& dir, const std::string& res) {
  Strings fileInfo = split(res, '\n');
  Strings files;
  for (const auto& fi : fileInfo) {
    if (fi.size() <= FilePerm::DirModeFileOffset) continue;
    files.push_back(fi.substr(FilePerm::DirModeFileOffset));
  }
  auto& obj = getInstance();
  auto d = obj.noTrailingSlash(dir);
  obj.cache[d] = files;
}

Strings& DirCache::getDirContents(const std::string& dir) {
  auto& obj = getInstance();
  auto d = obj.noTrailingSlash(dir);
  auto itr = obj.cache.find(d);
  if (itr == obj.cache.end()) {
    obj.cache[d] = listDirRel(d);
    itr = obj.cache.find(d);
  }
  return itr->second;
}

std::string DirCache::noTrailingSlash(const std::string& dir) {
  auto d = dir;
  // no trailing '/'
  if (d.size() > 1 && d.back() == '/') d.erase(d.end() - 1);
  return d;
}


FilesHist::FilesHist(const std::string& f, size_t max): file(f), maxLen(max) {
  if(!isFile(file)) return;
  auto arr = slurpToArr(file);
  for(const auto& a : arr) push_back(readFileInfo(a));
  prune();
}

void FilesHist::prune() {
  if(size() > maxLen) erase(begin()+maxLen, end());
}

void FilesHist::store() const {
  if(empty()) return;
  FILE* fp = fopen(file.c_str(), "w");
  if(fp == NULL) return;
  for(const auto& fi : *this)
    fprintf(fp, "%s;%lu\n", fi.first.c_str(), fi.second);
  fclose(fp);
}

void FilesHist::add(const std::string& file, size_t line) {
  FileInfo fi(file, line);
  // remove duplicates
  for (size_t i = 0; i < size(); ++i) {
    if(at(i).first == file) {
      erase(begin()+i);
      --i;
    }
  }
  insert(begin(), fi);
  prune();
}

Strings FilesHist::toString() const {
  Strings vec;
  for(const auto& fi : *this)
    vec.push_back(format("%s;%lu", fi.first.c_str(), fi.second));
  return vec;
}

} // end namespace teditor
