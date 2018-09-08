#include <sys/stat.h>
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
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>


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
    if(ta.tv_sec > tb.tv_sec)
        return true;
    if((ta.tv_sec == tb.tv_sec) && (ta.tv_usec > tb.tv_usec))
        return true;
    return false;
}

bool isDir(const char* f) {
    struct stat st;
    if(stat(f, &st) != 0)
        return false;
    return S_ISDIR(st.st_mode);
}

bool isFile(const char* f) {
    struct stat st;
    if(stat(f, &st) != 0)
        return false;
    return S_ISREG(st.st_mode);
}

bool isReadOnly(const char* f) {
    return (access(f, R_OK) == 0) && (access(f, W_OK) < 0);
}

bool dirExists(const std::string& f) {
    return !isFile(f.c_str()) && isDir(f.c_str());
}

void makeDir(const std::string& d) {
    if(dirExists(d))
        return;
    int ret = mkdir(d.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    ASSERT(ret >= 0, "makeDir: '%s' failed!", d.c_str());
}

bool isRemote(const char* f) {
    return !strncmp(f, "/ssh:", 5);
}

std::string env(const char* s) {
    auto* val = getenv(s);
    return val == nullptr? "" : val;
}

std::string env(const std::string& s) {
    return env(s.c_str());
}

std::string slurp(const char* file) {
    FILE *f = fopen(file, "rb");
    ASSERT(f, "Failed to open file '%s'!", file);
    struct stat st;
    bool status = fstat(fileno(f), &st) == 0;
    if(!status)
        fclose(f);
    ASSERT(status, "Failed to fstat file '%s'!", file);
    auto len = static_cast<size_t>(st.st_size);
    std::string data(len, '\0');
    status = fread(&(data[0]), 1, st.st_size, f) == len;
    if(!status)
        fclose(f);
    ASSERT(status, "Failed to fread file '%s'!", file);
    fclose(f);
    return data;
}

std::string slurp(const std::string& file) {
    return slurp(file.c_str());
}

Strings slurpToArr(const char* file) {
    Strings ret;
    std::fstream fp;
    fp.open(file, std::fstream::in);
    ASSERT(fp.is_open(), "Failed to open file '%s'!", file);
    std::string currLine;
    while(std::getline(fp, currLine, '\n')) {
        ret.push_back(currLine);
    }
    return ret;
}

Strings slurpToArr(const std::string& file) {
    return slurpToArr(file.c_str());
}

bool startsWith(const char* s1, int len1, const char* s2) {
    for(int n=0;*s2 && (n < len1);++s1,++s2,++n) {
        if(*s1 != *s2)
            return false;
    }
    return *s2 == 0;
}

Strings split(const std::string &str, char delim) {
    std::stringstream ss(str);
    std::string item;
    Strings res;
    while(std::getline(ss, item, delim))
        res.push_back(item);
    return res;
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

std::string getpwd() {
    char cwd[2048];
    getcwd(cwd, 2048);
    return std::string(cwd);
}

std::string rel2abs(const std::string& pwd, const std::string& rel) {
    char abs[2048];
    auto currCwd = getpwd();
    chdir(pwd.c_str());
    realpath(rel.c_str(), abs);
    chdir(currCwd.c_str());
    std::string res(abs);
    if(res.empty())
        res = pwd + '/' + rel;
    return res;
}

std::string basename(const std::string& file) {
    std::string tmp(file);
    if(tmp.back() == '/')
        tmp.pop_back();
    return tmp.substr(tmp.find_last_of("/\\") + 1);
}

std::string dirname(const std::string& file) {
    auto tmp = file;
    if(file == "/")
        return "/";
    if(!tmp.empty() && tmp.back() == '/')
        tmp.pop_back();
    auto loc = tmp.find_last_of("/\\");
    if(loc == std::string::npos)
        return "";
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

std::string uniquifyName(const std::string& name,
                         const std::set<std::string>& list) {
    std::string out(name);
    auto start=list.begin(), end=list.end();
    int idx = 0;
    while(std::find(start, end, out) != end) {
        ++idx;
        out = name + num2str(idx);
    }
    return out;
}

std::string findFirstUpwards(const std::string& dir, const std::string& file) {
    if(dir.empty() || file.empty())
        return "";
    auto d = dir;
    if(d.back() == '/')
        d.pop_back();
    auto f = d + '/' + file;
    if(isFile(f.c_str()) || isDir(f.c_str()))
        return f;
    while(!d.empty() && d != "/") {
        d = dirname(d);
        f = d + '/' + file;
        if(isFile(f.c_str()) || isDir(f.c_str()))
            return f;
    }
    return "";
}

bool isUnderGit(const std::string& dir) {
    auto root = findFirstUpwards(dir, ".git");
    return !root.empty();
}

std::string gitBranchName(const std::string& dir) {
    if(!isUnderGit(dir))
        return "";
    std::string cmdStr = "cd " + dir + " && git rev-parse --abbrev-ref HEAD";
    // Note: this line consumes non-trivial amount of time!
    auto cmd = check_output(cmdStr);
    if(cmd.status != 0)
        return "";
    auto ret = cmd.output;
    if(ret.back() == '\n')
        ret = ret.substr(0, ret.size()-1);
    return ret;
}

std::string extension(const std::string& name) {
    auto loc = name.find_last_of('.');
    if(loc == std::string::npos)
        return "";
    return name.substr(loc+1);
}

CmdStatus check_output(const std::string& cmd) {
    CmdStatus ret;
    if(cmd.back() == '&') {
        ret.status = 0;
        system(cmd.c_str());
        return ret;
    }
    const int MaxBuff = 8192;
    int pfd[2];
    int status = pipe(pfd);
    ASSERT(status >= 0, "Pipe failed for command '%s' [status=%d]!",
           cmd.c_str(), status);
    int pid = fork();
    if(pid < 0) {
        ASSERT(false, "Fork failed for command '%s'!", cmd.c_str());
    } else if(pid == 0) {
        // child
        close(pfd[0]);
        dup2(pfd[1], STDOUT_FILENO);
        dup2(pfd[1], STDERR_FILENO);
        execlp("/bin/bash", "-i", "-c", cmd.c_str(), NULL);
        exit(-1);
    } else {
        // parent
        close(pfd[1]);
    }
    char buf[MaxBuff];
    int nread;
    while((nread = read(pfd[0], buf, MaxBuff)) > 0) {
        ret.output += std::string(buf, nread);
    }
    waitpid(pid, &status, WUNTRACED);
    ret.status = WEXITSTATUS(status);
    close(pfd[0]);
    return ret;
}


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
    if(dp == nullptr)
        return f;
    for(struct dirent *ep=readdir(dp);ep!=nullptr;ep=readdir(dp)) {
        std::string relFile(ep->d_name);
        std::string file(dir);
        if(file.back() != '/')
            file += '/';
        file += relFile;
        FilePerm fp(file);
        if(relFile == "." || relFile == "..")
            fp.name = relFile;
        f.push_back(fp);
    }
    closedir(dp);
    sort(f.begin(), f.end(), FileCompare);
    return f;
}

Strings listDirRel(const std::string& dir) {
    Strings f;
    DIR *dp = opendir(dir.c_str());
    if(dp == nullptr)
        return f;
    for(struct dirent *ep=readdir(dp);ep!=nullptr;ep=readdir(dp))
        f.push_back(ep->d_name);
    closedir(dp);
    sort(f.begin(), f.end());
    return f;
}

void copyFile(const std::string& in, const std::string& out) {
    struct stat st;
    ASSERT(stat(in.c_str(), &st) == 0, "'stat' failed on '%s'!", in.c_str());
    int ifp = open(in.c_str(), O_RDONLY, 0);
    int ofp = open(out.c_str(), O_WRONLY | O_CREAT, st.st_mode);
    size_t size;
    char buf[BUFSIZ];
    while((size = read(ifp, buf, BUFSIZ)) > 0)
        write(ofp, buf, size);
    close(ifp);
    close(ofp);
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
            if(pos == std::string::npos)
                break;
            ret.replace(pos, name.size(), val);
        }
    }
    return ret;
}

FileInfo readFileInfo(const std::string& arg) {
    std::string file;
    int line;
    const auto tokens = split(arg, ':');
    if(tokens.size() == 1U) {
        file = tokens[0];
        line = 0;
    } else if(tokens.size() == 2U) {
        file = tokens[0];
        line = str2num(tokens[1]);
    } else {
        ASSERT(false, "File: Bad arg passed. Usage: <file>[:<line>]. '%s'!\n",
               arg.c_str());
    }
    return FileInfo(file, line);
}

bool strFind(const std::string& line, const std::string& str) {
    return line.find(str) != std::string::npos;
}

bool fileStrFind(const std::string& line, const std::string& str) {
    auto loc = str.find_last_of('/');
    // this shouldn't happen in reality, though!
    if(loc == std::string::npos)
        return strFind(line, str);
    // pick the basename
    auto sub = str.substr(loc+1);
    if(sub.empty())
        return true;
    return strFind(line, sub);
}

} // end namespace teditor
