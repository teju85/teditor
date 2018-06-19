#include "utils.h"
#include <sys/time.h>
#include <time.h>
#include "gtest/gtest.h"

namespace teditor {

void sleep(int timeMs) {
    clock_t goal = timeMs + clock();
    while (goal > clock());
}

TEST(Utils, Timeval) {
    struct timeval ta, tb;
    gettimeofday(&ta, NULL);
    sleep(500);
    gettimeofday(&tb, NULL);
    ASSERT_TRUE(tb > ta);
    ASSERT_FALSE(ta > tb);
    ASSERT_FALSE(ta > ta);
}

TEST(Utils, isDir) {
    ASSERT_TRUE(isDir("src"));
    ASSERT_FALSE(isDir("README.org"));
    ASSERT_FALSE(isDir("I_dont_exist"));
}

TEST(Utils, isFile) {
    ASSERT_TRUE(isFile("README.org"));
    ASSERT_FALSE(isFile("src"));
    ASSERT_FALSE(isFile("I_dont_exist"));
}

TEST(Utils, isReadOnly) {
    ASSERT_TRUE(isReadOnly("tests/samples/read-only.txt"));
    ASSERT_FALSE(isReadOnly("tests/samples/hello.txt"));
}

TEST(Utils, isRemote) {
    ASSERT_TRUE(isRemote("/ssh:cluster:/path"));
    ASSERT_FALSE(isRemote("tests/samples/hello.txt"));
}

TEST(Utils, Assert) {
    ASSERT_THROW(ASSERT(false, "Fail!"), std::runtime_error);
    ASSERT_NO_THROW(ASSERT(true, "Fail!"));
}

TEST(Utils, Env) {
    ASSERT_NO_THROW(env("TERM"));
}

TEST(Utils, Slurp) {
    ASSERT_EQ("Hello World!\n", slurp("tests/samples/hello.txt"));
    ASSERT_THROW(slurp("i_dont_exist"), std::runtime_error);
}

TEST(Utils, StartsWith) {
    ASSERT_TRUE(startsWith("Hello World!", 12, "Hell"));
    ASSERT_FALSE(startsWith("Hello World!", 12, "ell"));
    ASSERT_FALSE(startsWith("Hello World!", 12, "Hello World! John Doe"));
}

TEST(Utils, Num2Str) {
    ASSERT_EQ("123", num2str(123));
    ASSERT_EQ("-123", num2str(-123));
}

TEST(Utils, Str2Num) {
    ASSERT_EQ(123, str2num("123"));
    ASSERT_EQ(-123, str2num("-123"));
    ASSERT_EQ(0, str2num("fdsr324"));
}

TEST(Utils, Split) {
    std::vector<std::string> vec = split("abc:def:ghi", ':');
    ASSERT_EQ(3U, vec.size());
    ASSERT_EQ("abc", vec[0]);
    ASSERT_EQ("def", vec[1]);
    ASSERT_EQ("ghi", vec[2]);

    vec = split("abc:def:", ':');
    ASSERT_EQ(2U, vec.size());
    ASSERT_EQ("abc", vec[0]);
    ASSERT_EQ("def", vec[1]);

    vec = split(":abc:def", ':');
    ASSERT_EQ(3U, vec.size());
    ASSERT_EQ("", vec[0]);
    ASSERT_EQ("abc", vec[1]);
    ASSERT_EQ("def", vec[2]);

    vec = split("abc::def", ':');
    ASSERT_EQ(3U, vec.size());
    ASSERT_EQ("abc", vec[0]);
    ASSERT_EQ("", vec[1]);
    ASSERT_EQ("def", vec[2]);
}

TEST(Utils, Rel2Abs) {
    char buf[2048];
    getcwd(buf, 2048);
    std::string pwd(buf);
    ASSERT_EQ(pwd+"/README.org", rel2abs(".", "README.org"));
    ASSERT_EQ(pwd+"/tests/utils.cpp", rel2abs("tests/", "utils.cpp"));
    ASSERT_EQ(pwd+"/main.cpp", rel2abs("tests/", "../main.cpp"));
    ASSERT_EQ("./nofile", rel2abs(".", "nofile"));
    ASSERT_EQ("/some/non/existent/path/../nofile",
              rel2abs("/some/non/existent/path", "../nofile"));
    ASSERT_EQ("/usr/include", rel2abs(".", "/usr/include"));
}

TEST(Utils, Basename) {
    ASSERT_EQ("file.txt", basename("/root/file.txt"));
    ASSERT_EQ("file.txt", basename("/root///fds/fds/../file.txt"));
    ASSERT_EQ("file.txt", basename("fds/file.txt"));
    ASSERT_EQ("testdir", basename("fds/testdir/"));
    ASSERT_EQ("test", basename("test"));
}

TEST(Utils, Dirname) {
    ASSERT_EQ("/root", dirname("/root/file.txt"));
    ASSERT_EQ("/root///fds/fds/..", dirname("/root///fds/fds/../file.txt"));
    ASSERT_EQ("fds/testdir", dirname("fds/testdir/test/"));
    ASSERT_EQ("", dirname("fds"));
    ASSERT_EQ("/", dirname("/"));
}

TEST(Utils, UniquifyName) {
    ASSERT_EQ("scratch", uniquifyName("scratch", {}));
    ASSERT_EQ("scratch1", uniquifyName("scratch", {"scratch", "something"}));
    ASSERT_EQ("scratch1", uniquifyName("scratch",
                                       {"scratch", "something", "scratch2"}));
    ASSERT_EQ("scratch2", uniquifyName("scratch",
                                       {"scratch", "something", "scratch1"}));
}

TEST(Utils, CheckOutput) {
    auto out = check_output("cat tests/samples/hello.txt");
    ASSERT_EQ("Hello World!\n", out.output);
    ASSERT_EQ(0, out.status);
    out = check_output("something");
    ASSERT_EQ("-i: something: command not found\n", out.output);
    ASSERT_NE(0, out.status);
}

TEST(Utils, FilePerm) {
    FilePerm fp("tests/samples/hello.txt");
    ASSERT_EQ(0, strncmp(fp.perms, "-rw-r--r--", 10));
    FilePerm fp1("tests/samples/read-only.txt");
    ASSERT_EQ(0, strncmp(fp1.perms, "-r--r--r--", 10));
    ASSERT_EQ(13U, fp1.size);
    FilePerm fp2("tests/samples");
    ASSERT_EQ(0, strncmp(fp2.perms, "drwxr-xr-x", 10));
    ASSERT_EQ(0U, fp2.size);
}

TEST(Utils, Files) {
    Files f = listDir("tests/samples");
    ASSERT_EQ(5U, f.size());
    ASSERT_EQ(".", f[0].name);
    ASSERT_EQ("..", f[1].name);
    ASSERT_EQ("tests/samples/hello.txt", f[2].name);
    ASSERT_EQ("tests/samples/multiline.txt", f[3].name);
    ASSERT_EQ("tests/samples/read-only.txt", f[4].name);
    Files f1 = listDir("not-exists");
    ASSERT_EQ(0U, f1.size());
}

TEST(Utils, CopyFile) {
    const char* file = "tests/samples/hello.txt";
    const char* copy = "tmp.txt";
    auto ref = slurp(file);
    copyFile(file, copy);
    auto val = slurp(copy);
    remove(copy);
    ASSERT_EQ(ref, val);
}

TEST(Utils, Paren) {
    ASSERT_TRUE(isOpenParen('('));
    ASSERT_TRUE(isOpenParen('{'));
    ASSERT_TRUE(isOpenParen('['));
    ASSERT_TRUE(isOpenParen('<'));
    ASSERT_FALSE(isOpenParen('^'));
    ASSERT_TRUE(isCloseParen(')'));
    ASSERT_TRUE(isCloseParen('}'));
    ASSERT_TRUE(isCloseParen(']'));
    ASSERT_TRUE(isCloseParen('>'));
    ASSERT_FALSE(isCloseParen('^'));
}

TEST(Utils, ExpandEnvVars) {
    setenv("TEST", "test", 1);
    ASSERT_EQ("test/", expandEnvVars("$TEST/", {"TEST"}));
    ASSERT_EQ("test/some/test", expandEnvVars("$TEST/some/test", {"TEST"}));
    ASSERT_EQ("$TEST/", expandEnvVars("$TEST/", {"SOME"}));
}

} // end namespace teditor
