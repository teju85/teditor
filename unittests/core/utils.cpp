#include "core/utils.h"
#include "core/file_utils.h"
#include <sys/time.h>
#include <time.h>
#include "catch.hpp"
#include <string.h>


namespace teditor {

TEST_CASE("Utils::allStringsEmpty") {
    Strings strs;
    REQUIRE(allStringsEmpty(strs));
    strs.push_back(std::string(""));
    REQUIRE(allStringsEmpty(strs));
    strs.push_back(std::string());
    REQUIRE(allStringsEmpty(strs));
    strs.push_back(std::string("Hi"));
    REQUIRE_FALSE(allStringsEmpty(strs));
}

TEST_CASE("Utils::same") {
    Strings strs1, strs2;
    REQUIRE(same(strs1, strs2));
    strs1.push_back("one");
    REQUIRE_FALSE(same(strs1, strs2));
    strs2.push_back("one");
    REQUIRE(same(strs1, strs2));
    strs1.push_back("two");
    REQUIRE_FALSE(same(strs1, strs2));
    strs2.push_back("three");
    REQUIRE_FALSE(same(strs1, strs2));
}

void sleep(int timeMs) {
    clock_t goal = timeMs + clock();
    while (goal > clock());
}

bool compareTestFunc(struct timeval a, struct timeval b) {
    return a > b;
}

TEST_CASE("Utils::Timeval") {
    struct timeval ta, tb;
    gettimeofday(&ta, NULL);
    sleep(500);
    gettimeofday(&tb, NULL);
    REQUIRE(compareTestFunc(tb, ta));
    REQUIRE_FALSE(compareTestFunc(ta, tb));
    REQUIRE_FALSE(compareTestFunc(ta, ta));
}

TEST_CASE("Utils::isDir") {
    REQUIRE(isDir("src"));
    REQUIRE_FALSE(isDir("README.org"));
    REQUIRE_FALSE(isDir("I_dont_exist"));
}

TEST_CASE("Utils::isFile") {
    REQUIRE(isFile("README.org"));
    REQUIRE_FALSE(isFile("src"));
    REQUIRE_FALSE(isFile("I_dont_exist"));
}

TEST_CASE("Utils::isRemote") {
    REQUIRE(isRemote("/ssh:cluster:/path"));
    REQUIRE_FALSE(isRemote("samples/hello.txt"));
}

void assertTestFunc(bool flag) {
    ASSERT(flag, "Fail!");
}

TEST_CASE("Utils::Assert") {
    REQUIRE_THROWS(assertTestFunc(false));
    REQUIRE_NOTHROW(assertTestFunc(true));
}

TEST_CASE("Utils::Env") {
    REQUIRE_NOTHROW(env("TERM"));
}

TEST_CASE("Utils::Slurp") {
    REQUIRE("Hello World!\n" == slurp("samples/hello.txt"));
    REQUIRE_THROWS(slurp("i_dont_exist"));
}

TEST_CASE("Utils::SlurpToArr") {
    auto arr = slurpToArr("samples/multiline.txt");
    REQUIRE(3U == arr.size());
    REQUIRE("* Hello" == arr[0]);
    REQUIRE("Testing123" == arr[1]);
    REQUIRE("for multi-line buffer!" == arr[2]);
}

TEST_CASE("Utils::StartsWith") {
    REQUIRE(startsWith("Hello World!", 12, "Hell"));
    REQUIRE_FALSE(startsWith("Hello World!", 12, "ell"));
    REQUIRE_FALSE(startsWith("Hello World!", 12, "Hello World! John Doe"));
}

TEST_CASE("Utils::Num2Str") {
    REQUIRE("123" == num2str(123));
    REQUIRE("-123" == num2str(-123));
}

TEST_CASE("Utils::Str2Num") {
    REQUIRE(123 == str2num("123"));
    REQUIRE(-123 == str2num("-123"));
    REQUIRE(0 == str2num("fdsr324"));
}

TEST_CASE("Utils::Str2Real") {
    REQUIRE(123.f == str2real("123."));
    REQUIRE(-123.f == str2real("-123"));
    REQUIRE(0 == str2real("fdsr324"));
}

TEST_CASE("Utils::Str2Double") {
    REQUIRE(123.0 == str2double("123."));
    REQUIRE(-123.0 == str2double("-123"));
    REQUIRE(0 == str2double("fdsr324"));
}

TEST_CASE("Utils::Split") {
    std::vector<std::string> vec = split("abc:def:ghi", ':');
    REQUIRE(3U == vec.size());
    REQUIRE("abc" == vec[0]);
    REQUIRE("def" == vec[1]);
    REQUIRE("ghi" == vec[2]);

    vec = split("abc:def:", ':');
    REQUIRE(2U == vec.size());
    REQUIRE("abc" == vec[0]);
    REQUIRE("def" == vec[1]);

    vec = split(":abc:def", ':');
    REQUIRE(3U == vec.size());
    REQUIRE("" == vec[0]);
    REQUIRE("abc" == vec[1]);
    REQUIRE("def" == vec[2]);

    vec = split("abc::def", ':');
    REQUIRE(3U == vec.size());
    REQUIRE("abc" == vec[0]);
    REQUIRE("" == vec[1]);
    REQUIRE("def" == vec[2]);
}

TEST_CASE("Utils::Join") {
  auto tokens = split("abc:def:ghi", ':');
  REQUIRE("abc.def.ghi" == join(tokens, '.'));
  REQUIRE("def.ghi" == join(tokens, '.', 1));
}

TEST_CASE("Utils::isAbs") {
  REQUIRE(!isAbs(""));
  REQUIRE(!isAbs("test"));
  REQUIRE(isAbs("/home"));
}

TEST_CASE("Utils::Rel2Abs") {
    std::string pwd = getpwd();
    REQUIRE(pwd+"/README.org" == rel2abs(".", "README.org"));
    REQUIRE(pwd+"/unittests/core/utils.cpp" == rel2abs("unittests/core/", "utils.cpp"));
    REQUIRE(pwd+"/main.cpp" == rel2abs("unittests/", "../main.cpp"));
    REQUIRE("./nofile" == rel2abs(".", "nofile"));
    REQUIRE("/some/non/existent/path/../nofile" ==
            rel2abs("/some/non/existent/path", "../nofile"));
    REQUIRE("/usr/include" == rel2abs(".", "/usr/include"));
}

TEST_CASE("Utils::Basename") {
    REQUIRE("file.txt" == basename("/root/file.txt"));
    REQUIRE("file.txt" == basename("/root///fds/fds/../file.txt"));
    REQUIRE("file.txt" == basename("fds/file.txt"));
    REQUIRE("testdir" == basename("fds/testdir/"));
    REQUIRE("test" == basename("test"));
    REQUIRE("" == basename(""));
}

TEST_CASE("Utils::Dirname") {
    REQUIRE("/root" == dirname("/root/file.txt"));
    REQUIRE("/root///fds/fds/.." == dirname("/root///fds/fds/../file.txt"));
    REQUIRE("fds/testdir" == dirname("fds/testdir/test/"));
    REQUIRE("" == dirname("fds"));
    REQUIRE("/" == dirname("/"));
    REQUIRE("" == dirname(""));
}

TEST_CASE("Utils::FindFirstUpwards") {
    std::string pwd = getpwd();
    REQUIRE("" == findFirstUpwards("", ""));
    REQUIRE("" == findFirstUpwards("/", ""));
    REQUIRE(pwd + "/.git" == findFirstUpwards(pwd, ".git"));
    REQUIRE(pwd + "/README.org" == findFirstUpwards(pwd, "README.org"));
    REQUIRE(pwd + "/samples" == findFirstUpwards(pwd, "samples"));
    REQUIRE("" == findFirstUpwards(pwd, "non-existent"));
}

TEST_CASE("Utils::IsUnderGit") {
    REQUIRE(isUnderGit(getpwd()));
    REQUIRE_FALSE(isUnderGit("/"));
}

TEST_CASE("Utils::Extension") {
    REQUIRE("cpp" == extension("file.cpp"));
    REQUIRE("cpp" == extension("file.a.cpp"));
    REQUIRE("" == extension("file."));
    REQUIRE("" == extension("file"));
    REQUIRE("" == extension(""));
}

TEST_CASE("Utils::CheckOutput") {
  auto out = check_output("cat samples/hello.txt");
  REQUIRE("Hello World!\n" == out.output);
  REQUIRE("" == out.error);
  REQUIRE(0 == out.status);
  out = check_output("something");
  REQUIRE("-i: something: command not found\n" == out.error);
  REQUIRE("" == out.output);
  REQUIRE(0 != out.status);
  out = check_output("echo Error 1>&2; echo Output");
  REQUIRE("Output\n" == out.output);
  REQUIRE("Error\n" == out.error);
  REQUIRE(0 == out.status);
}

TEST_CASE("Utils::FilePerm") {
    FilePerm fp("samples/hello.txt");
    REQUIRE(0 == strncmp(fp.perms, "-rw-r--r--", 10));
    FilePerm fp1("samples");
    REQUIRE(0 == strncmp(fp1.perms, "drwxr-xr-x", 10));
    REQUIRE(0U == fp1.size);
}

TEST_CASE("Utils::ListDir") {
    auto f = listDir("samples");
    REQUIRE(13U == f.size());
    REQUIRE("." == f[0].name);
    REQUIRE(".." == f[1].name);
    REQUIRE("samples/correct.cpp" == f[2].name);
    REQUIRE("samples/cpp-mode.cpp" == f[3].name);
    REQUIRE("samples/default.rcfile" == f[4].name);
    REQUIRE("samples/hello.txt" == f[5].name);
    REQUIRE("samples/incorrect.cpp" == f[6].name);
    REQUIRE("samples/incorrect.rcfile" == f[7].name);
    REQUIRE("samples/indent.txt" == f[8].name);
    REQUIRE("samples/ledger" == f[9].name);
    REQUIRE("samples/long.cpp" == f[10].name);
    REQUIRE("samples/multiline.txt" == f[11].name);
    REQUIRE("samples/sample.cxx" == f[12].name);
    auto f1 = listDir("not-exists");
    REQUIRE(0U == f1.size());
}

TEST_CASE("Utils::ListDirRel") {
    auto f = listDirRel("samples");
    REQUIRE(13U == f.size());
    REQUIRE("." == f[0]);
    REQUIRE(".." == f[1]);
    REQUIRE("correct.cpp" == f[2]);
    REQUIRE("cpp-mode.cpp" == f[3]);
    REQUIRE("default.rcfile" == f[4]);
    REQUIRE("hello.txt" == f[5]);
    REQUIRE("incorrect.cpp" == f[6]);
    REQUIRE("incorrect.rcfile" == f[7]);
    REQUIRE("indent.txt" == f[8]);
    REQUIRE("ledger" == f[9]);
    REQUIRE("long.cpp" == f[10]);
    REQUIRE("multiline.txt" == f[11]);
    REQUIRE("sample.cxx" == f[12]);
    auto f1 = listDirRel("not-exists");
    REQUIRE(0U == f1.size());
}

TEST_CASE("Utils::CopyFile") {
    const char* file = "samples/hello.txt";
    const char* copy = "tmp.txt";
    auto ref = slurp(file);
    copyFile(file, copy);
    auto val = slurp(copy);
    remove(copy);
    REQUIRE(ref == val);
}

TEST_CASE("Utils::Paren") {
    REQUIRE(isOpenParen('('));
    REQUIRE(isOpenParen('{'));
    REQUIRE(isOpenParen('['));
    REQUIRE(isOpenParen('<'));
    REQUIRE_FALSE(isOpenParen('^'));
    REQUIRE(isCloseParen(')'));
    REQUIRE(isCloseParen('}'));
    REQUIRE(isCloseParen(']'));
    REQUIRE(isCloseParen('>'));
    REQUIRE_FALSE(isCloseParen('^'));
}

TEST_CASE("Utils::ExpandEnvVars") {
  setenv("TEST", "test", 1);
  REQUIRE("test/" == expandEnvVars("$TEST/"));
  REQUIRE("test/some/test" == expandEnvVars("$TEST/some/test"));
  REQUIRE("/" == expandEnvVars("$TEST1/"));
  REQUIRE("test" == expandEnvVars("$TEST"));
}

TEST_CASE("Utils::ReadFileInfo") {
  FileInfo fi = readFileInfo("README.org;10");
  REQUIRE("README.org" == fi.first);
  REQUIRE(10 == fi.second);
  REQUIRE_THROWS(readFileInfo("README.org;10;10"));
  fi = readFileInfo("/ssh:remotehost:/usr/README.org;11");
  REQUIRE("/ssh:remotehost:/usr/README.org" == fi.first);
  REQUIRE(11 == fi.second);
}

TEST_CASE("Utils::StrFind") {
    REQUIRE(strFind("there is abc in this line", "abc"));
    REQUIRE_FALSE(strFind("there is no", "abc"));
    REQUIRE_FALSE(strFind("abc", "no matching"));
}

TEST_CASE("Utils::FileStrFind") {
    REQUIRE(fileStrFind("files", "file"));
    REQUIRE_FALSE(fileStrFind("files", "tile"));
    REQUIRE(fileStrFind("file", "/home/user/"));
    REQUIRE_FALSE(fileStrFind("file", "/home/user/check"));
    REQUIRE(fileStrFind("file", "/home/user/fil"));
}

TEST_CASE("Utils::IsCurrentOrParentDir") {
    REQUIRE(isCurrentOrParentDir("."));
    REQUIRE(isCurrentOrParentDir(".."));
    REQUIRE_FALSE(isCurrentOrParentDir("other"));
}

TEST_CASE("Utils::urlHexify") {
  REQUIRE(urlHexify("abcdefghijklmnopqrtsuvwxyz") ==
          "abcdefghijklmnopqrtsuvwxyz");
  REQUIRE(urlHexify("ABCDEFGHIJKLMNOPQRSTUVXWYZ") ==
          "ABCDEFGHIJKLMNOPQRSTUVXWYZ");
  REQUIRE(urlHexify("0123456789") == "0123456789");
  REQUIRE(urlHexify("`~!@# ") == "%60%7e%21%40%23%20");
  REQUIRE(urlHexify("abc`~!@#def") == "abc%60%7e%21%40%23def");
  REQUIRE(urlHexify("testing c") == "testing%20c");
}

TEST_CASE("Remote") {
  Remote r("/ssh:host:/full/path");
  REQUIRE(r.host == "host");
  REQUIRE(r.file == "/full/path");
  REQUIRE_THROWS_AS(Remote("/fsdfds:fdsfd:rerw"), std::runtime_error);
  REQUIRE_THROWS_AS(Remote("/ssh:fdsf"), std::runtime_error);
}

} // end namespace teditor
