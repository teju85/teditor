#include "utils.h"
#include <cstdlib>
#include <cstring>
#include "utf8.h"
#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <cctype>
#include "file_utils.h"


namespace teditor {

std::string format(const char* fmt, va_list& vl) {
  char buf[4096];
  vsnprintf(buf, sizeof(buf), fmt, vl);
  return std::string(buf);
}

std::string format(const char* fmt, ...) {
  va_list vl;
  va_start(vl, fmt);
  std::string str = format(fmt, vl);
  va_end(vl);
  return str;
}

bool operator>(const struct timeval& ta, const struct timeval& tb) {
  if(ta.tv_sec > tb.tv_sec) return true;
  if((ta.tv_sec == tb.tv_sec) && (ta.tv_usec > tb.tv_usec)) return true;
  return false;
}

std::string env(const char* s) {
  auto* val = getenv(s);
  return val == nullptr? "" : val;
}

std::string env(const std::string& s) {
  return env(s.c_str());
}

bool startsWith(const char* s1, int len1, const char* s2) {
  for(int n=0;*s2 && (n < len1);++s1,++s2,++n) {
    if(*s1 != *s2) return false;
  }
  return *s2 == 0;
}

Strings split(const std::string &str, char delim) {
  std::stringstream ss(str);
  std::string item;
  Strings res;
  while(std::getline(ss, item, delim)) res.push_back(item);
  return res;
}

std::string join(const Strings &s, char delim, int start, int end) {
  std::string ret;
  if(end < 0) end = (int)s.size();
  for(int i=start;i<end;++i) {
    if(i > start) ret += delim;
    ret += s[i];
  }
  return ret;
}

std::string num2str(int num) {
  std::stringstream ss;
  ss << num;
  std::string str = ss.str();
  return str;
}

int str2num(const std::string& str) {
  std::stringstream ss(str);
  int out = -1;
  ss >> out;
  return out;
}

float str2real(const std::string& str) {
  std::stringstream ss(str);
  float out = -1.f;
  ss >> out;
  return out;
}

double str2double(const std::string& str) {
  std::stringstream ss(str);
  double out = -1.0;
  ss >> out;
  return out;
}

bool isUnderGit(const std::string& dir) {
  auto root = findFirstUpwards(dir, ".git");
  return !root.empty();
}

std::string gitBranchName(const std::string& dir) {
  if(!isUnderGit(dir)) return "";
  std::string cmdStr = "cd " + dir + " && git rev-parse --abbrev-ref HEAD";
  // Note: this line consumes non-trivial amount of time!
  auto cmd = check_output(cmdStr);
  if(cmd.status != 0) return "";
  auto ret = cmd.output;
  if(ret.back() == '\n') ret = ret.substr(0, ret.size()-1);
  return ret;
}

CmdStatus check_output(const std::string& cmd) {
  CmdStatus ret;
  if(cmd.back() == '&') {
    ret.status = 0;
    (void)system(cmd.c_str());
    return ret;
  }
  const int MaxBuff = 8192;
  int out_fd[2], err_fd[2];
  int status = pipe(out_fd);
  ASSERT(status >= 0, "Pipe failed for command '%s' [status=%d]!",
         cmd.c_str(), status);
  status = pipe(err_fd);
  if(status < 0) {
    close(out_fd[0]);
    close(out_fd[1]);
    ASSERT(false, "Pipe failed for command '%s' [status=%d]!",
           cmd.c_str(), status);
  }
  int pid = fork();
  if(pid < 0) {
    ASSERT(false, "Fork failed for command '%s'!", cmd.c_str());
    close(out_fd[0]);
    close(out_fd[1]);
    close(err_fd[0]);
    close(err_fd[1]);
  } else if(pid == 0) {
    // child
    close(out_fd[0]);
    close(err_fd[0]);
    dup2(out_fd[1], STDOUT_FILENO);
    dup2(err_fd[1], STDERR_FILENO);
    execlp("/bin/bash", "-i", "-c", cmd.c_str(), NULL);
    exit(-1);
  } else {
    // parent
    close(out_fd[1]);
    close(err_fd[1]);
  }
  char buf[MaxBuff];
  int nread;
  ret.output.clear();
  while((nread = read(out_fd[0], buf, MaxBuff)) > 0)
    ret.output += std::string(buf, nread);
  ret.error.clear();
  while((nread = read(err_fd[0], buf, MaxBuff)) > 0)
    ret.error += std::string(buf, nread);
  waitpid(pid, &status, WUNTRACED);
  ret.status = WEXITSTATUS(status);
  close(out_fd[0]);
  close(err_fd[1]);
  return ret;
}


std::string urlHexify(const std::string& url) {
  std::string ret;
  ret.reserve(url.size() * 3);
  for(auto c : url) {
    if(std::isalnum(c) || ':' == c || '/' == c || '.' == c) ret += c;
    else ret += format("%%%02x", c);
  }
  return ret;
}

void downloadUrl(const std::string& url, const std::string& file) {
  auto res = check_output("curl -s " + url + " > " + file);
  ASSERT(res.status == 0, "downloadUrl: failed! status=%d\n", res.status);
}

std::string downloadUrlToString(const std::string& url) {
  auto res = check_output("curl -s " + url);
  ASSERT(res.status == 0, "downloadUrl: failed! status=%d\n", res.status);
  return res.output;
}

bool isOpenParen(char c) {
  return (c == '(') || (c == '{') || (c == '[') || (c == '<');
}

bool isCloseParen(char c) {
  return (c == ')') || (c == '}') || (c == ']') || (c == '>');
}

bool isParen(char c) {
  return isOpenParen(c) || isCloseParen(c);
}

char getMatchingParen(char c) {
  switch(c) {
  case '{': return '}';
  case '[': return ']';
  case '(': return ')';
  case '<': return '>';
  case '}': return '{';
  case ']': return '[';
  case ')': return '(';
  case '>': return '<';
  default:  return c;
  };
}

std::string expandEnvVars(const std::string& str, const Strings& vars) {
  std::string ret(str);
  for(const auto& var : vars) {
    auto val = env(var);
    auto name = "$" + var;
    size_t pos = 0;
    while(pos < ret.size()) {
      pos = ret.find(name, pos);
      if(pos == std::string::npos) break;
      ret.replace(pos, name.size(), val);
    }
  }
  return ret;
}

bool strFind(const std::string& line, const std::string& str) {
  return line.find(str) != std::string::npos;
}

} // end namespace teditor
