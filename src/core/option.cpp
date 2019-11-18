#include <fstream>
#include <unordered_map>
#include "option.h"
#include <memory>
#include <string.h>
#include "logger.h"

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
  for (const auto& itr : opts) {
    auto opt = itr.second;
    fp << "# " << opt->helpMsg << "\n";
    fp << "# Type: " << Option::type2str(opt->type) << "\n";
    fp << itr.first << " " << opt->value << "\n\n";
  }
  fp.close();
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
  Option::add("cmBar::height", "1", "Cmd-bar height", Option::Type::Integer);
  Option::add("cmBar::multiheight", "8", "Cmd-bar height during interaction",
              Option::Type::Integer);
  Option::add("histFile", "<homeFolder>/history", "Path to history file",
              Option::Type::String);
  Option::add("homeFolder", "$HOME/.teditor", "Path to editor home folder",
              Option::Type::String);
  Option::add("ledgerFile", "<homeFolder>/ledger.lg",
              "Path to the ledger file used for personal accounting",
              Option::Type::String);
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
  Option::add("tabSpaces", "2", "Number of spaces per tab",
              Option::Type::Integer);
  Option::add("tty", "/dev/tty", "Path to the tty file", Option::Type::String);
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
    ///@todo: parse the rc file
  }
  fp.close();
}

std::vector<FileInfo> parseArgs(int argc, char** argv) {
  registerAllOptions();
  std::vector<FileInfo> files;
  for(int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-h")) {
      ///@todo: print help
    } else if (!strcmp(argv[i], "-dump")) {
      ++i;
      ASSERT(i < argc, "'-dump' option expects an argument!");
      Option::dumpAll(argv[i]);
    } else if(!strcmp(argv[i], "-rc")) {
      ++i;
      ASSERT(i < argc, "'-rc' option expects an argument!");
      parseRcFile(argv[i]);
    } else {
      ASSERT(argv[i][0] != '-', "Invalid arg passed! '%s'", argv[i]);
      files.push_back(readFileInfo(argv[i]));
    }
  }
  Logger::setLevel(Option::get("logLevel").getInt());
  makeDir(Option::get("homeFolder").getStr());
  return files;
}

}  // namespace teditor
