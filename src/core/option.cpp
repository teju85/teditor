#include <fstream>
#include <unordered_map>
#include "option.h"
#include <memory>
#include <string.h>
#include "utils.h"

namespace teditor {

std::string Option::expandOptions(const std::string& str) {
  std::string ret(str);
  for (size_t pos = 0; pos < ret.size(); ) {
    if (ret[pos] == '<') {
      auto loc = ret.find_first_of(">", pos + 1);
      auto optName = ret.substr(pos + 1, loc - pos - 1);
      auto val = get(optName).getStr();
      ret.replace(pos, loc - pos + 1, val);
      pos = loc;
      continue;
    }
    ++pos;
  }
  return ret;
}

std::string Option::getStr() const {
  ASSERT(type == String, "Option '%s' is not String!", name.c_str());
  auto ret = expandEnvVars(value);
  ret = expandOptions(ret);
  return ret;
}

typedef std::unordered_map<std::string, std::shared_ptr<Option>> Options;

Options& options() {
  static Options _options;
  return _options;
}

Option& Option::get(const std::string& name) {
  auto& opt = options();
  ASSERT(opt.find(name) != opt.end(), "Unable to find the option '%s'",
         name.c_str());
  return *opt[name];
}

void Option::set(const std::string& name, const std::string& val) {
  get(name).setVal(val);
}

void Option::add(const std::string& n, const std::string& v,
                 const std::string& hm, Option::Type t) {
  auto& opt = options();
  ASSERT(opt.find(n) == opt.end(), "Option '%s' already registered", n.c_str());
  opt[n].reset(new Option(n, v, hm, t));
}

void Option::dumpAll(const std::string& outfile) {
  std::ofstream fp;
  fp.open(outfile.c_str());
  ASSERT(fp.is_open(), "Failed to open file '%s'!", outfile.c_str());
  auto& opts = options();
  auto keys = sortedKeys(opts);
  for (const auto& k : keys) {
    auto opt = opts[k];
    fp << "# " << opt->helpMsg << "\n";
    fp << "# Type: " << Option::type2str(opt->type) << "\n";
    fp << k << " " << opt->value << "\n\n";
  }
  fp.close();
}

void Option::printOpts() {
  auto& opts = options();
  auto keys = sortedKeys(opts);
  std::string filler(25, ' ');
  printf("List of all supported options are:\n");
  printf(" . Value inside square braces is the data-type of the option.\n");
  printf(" . Value inside curly braces show the value of the option.\n");
  printf("------------------\n");
  for (const auto& k : keys) {
    auto opt = opts[k];
    auto tstr = Option::type2str(opt->type);
    printf("%-25s %s\n", k.c_str(), opt->helpMsg.c_str());
    printf("%s [%s] {%s}\n", filler.c_str(), tstr.c_str(), opt->value.c_str());
  }
  printf("------------------\n");
}

#define CASE(t) case t: return #t
std::string Option::type2str(Option::Type t) {
  switch(t) {
  CASE(Boolean);
  CASE(Integer);
  CASE(Real);
  CASE(String);
  CASE(Char);
  default: ASSERT(false, "Bad option type '%d'", t);
  };
}
#undef CASE

// NOTE: keep the options in alphabetical order
void registerAllOptions() {
  Option::add("browserCmd", "cygstart firefox -private-window",
              "Command to fire up your favorite browser", Option::Type::String);
  Option::add("calc:prompt", "expr> ", "Expression prompt during calc-mode",
              Option::Type::String);
  Option::add("calc:lineSeparator", std::string(80, '~'),
              "Used for separating history from the prompt in calc-mode",
              Option::Type::String);
  Option::add("calc:histFile", "<homeFolder>/calc-history",
              "Path to history file for calc-mode commands",
              Option::Type::String);
  Option::add("calc:maxHistory", "50",
              "History size for storing all calc-mode commands",
              Option::Type::Integer);
  Option::add("cmBar:height", "1", "Cmd-bar height", Option::Type::Integer);
  Option::add("cmBar:multiheight", "8", "Cmd-bar height during interaction",
              Option::Type::Integer);
  Option::add("dnldProg", "curl", "Program used for downloading URLs",
              Option::Type::String);
  Option::add("dnldProgOpts", "-s", "Options passed to <dnldProg>",
              Option::Type::String);
  Option::add("editor:pollTimeoutMs", "50", "Input poll event timeout (in ms)",
              Option::Type::Integer);
  Option::add("grepCmd", "grep -nH -e ", "Default grep command for prompting",
              Option::Type::String);
  Option::add("histFile", "<homeFolder>/history", "Path to history file",
              Option::Type::String);
  Option::add("homeFolder", "$HOME/.teditor", "Path to editor home folder",
              Option::Type::String);
  Option::add("iCaseSearch", "NO", "Perform case insensitive searches",
              Option::Type::Boolean);
  Option::add("logLevel", "-1", "Spew verbosity. Higher value means more spews",
              Option::Type::Integer);
  Option::add("maxHistory", "25", "History size for storing all files visited",
              Option::Type::Integer);
  Option::add("orgNotesDir", "<homeFolder>/org",
              "Path to the dir containing the org notes", Option::Type::String);
  Option::add("pageScrollJump", "0.9",
              "How much of a jump to make during page scrolls",
              Option::Type::Real);
  Option::add("quitAfterLoad", "NO",
              "Quit after parsing cmdline args and loading input files",
              Option::Type::Boolean);
  Option::add("songsDir", "<homeFolder>/songs", "Path to dir containing songs",
              Option::Type::String);
  Option::add("startProg", "cygstart", "Program used to open special files",
              Option::Type::String);
  Option::add("tabSpaces", "2", "Number of spaces per tab",
              Option::Type::Integer);
  Option::add("title", "teditor", "Window title", Option::Type::String);
  Option::add("todo:file", "<homeFolder>/tasks.todo",
              "File containing list of tasks to todo and/or set reminders for",
              Option::Type::String);
  Option::add("tty", "/dev/tty", "Path to the tty file", Option::Type::String);
  Option::add("watch:defaultSleepMs", "1000",
              "Default sleep duration (in ms) between reruns in watch-mode",
              Option::Type::Integer);
  Option::add("windowSplitter", "|", "Character used as window splitter",
              Option::Type::Char);
}

void parseRcFile(const std::string& rc) {
  std::fstream fp;
  fp.open(rc.c_str(), std::fstream::in);
  ASSERT(fp.is_open(), "Failed to open the input rc file '%s' for reading",
         rc.c_str());
  std::string currLine;
  while(std::getline(fp, currLine, '\n')) {
    if (currLine.empty()) continue;
    if (currLine[0] == '#') continue;
    auto pos = currLine.find(' ');
    ASSERT(pos != std::string::npos, "Incorrect syntax for rc file. Line:'%s'",
           currLine.c_str());
    auto optName = currLine.substr(0, pos);
    auto optVal = currLine.substr(pos + 1, currLine.size() - pos - 1);
    Option::set(optName, optVal);
  }
  fp.close();
}

bool parseArgs(int argc, char** argv, std::vector<FileInfo>& files) {
  registerAllOptions();
  for(int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-h")) {
      printf("teditor: Terminal based editor written in C++.\n"
             "USAGE:\n"
             "  teditor [-h] [-dump <rcfile>] [-rc <rcFile>] <files>\n"
             "OPTIONS:\n"
             "  -h               Print this help and exit.\n"
             "  -dump <rcFile>   Dump the default rc file and exit.\n"
             "  -rc <rcFile>     Configure the editor using this rc file.\n"
             "  -listopts        List all options supported and exit.\n"
             "  -o <name> <val>  Set the option as found in the 'Option'.\n"
             "  -v               Print version info and exit.\n"
             "  <files>          Files to be opened\n");
      return false;
    } else if (!strcmp(argv[i], "-dump")) {
      ++i;
      ASSERT(i < argc, "'-dump' option expects an argument!");
      Option::dumpAll(argv[i]);
      printf("rc file dumped at '%s'\n", argv[i]);
      return false;
    } else if(!strcmp(argv[i], "-rc")) {
      ++i;
      ASSERT(i < argc, "'-rc' option expects an argument!");
      parseRcFile(argv[i]);
    } else if(!strcmp(argv[i], "-v")) {
      printf("teditor version %s\n", TEDITOR_VERSION_INFO);
      return false;
    } else if(!strcmp(argv[i], "-listopts")) {
      Option::printOpts();
      return false;
    } else if(!strcmp(argv[i], "-o")) {
      ++i;
      ASSERT(i < argc, "'-o' option expects 2 arguments!");
      std::string optName(argv[i]);
      ++i;
      ASSERT(i < argc, "'-o' option expects 2 arguments!");
      std::string optVal(argv[i]);
      Option::set(optName, optVal);
    } else {
      ASSERT(argv[i][0] != '-', "Invalid arg passed! '%s'", argv[i]);
      files.push_back(readFileInfo(argv[i]));
    }
  }
  return true;
}

}  // namespace teditor
