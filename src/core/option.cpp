#include <fstream>
#include <unordered_map>
#include "option.h"
#include <memory>
#include <string.h>

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
  default: ASSERT(false, "Bad option type '%d'", t);
  };
}
#undef CASE

// NOTE: keep the options in alphabetical order
void registerAllOptions() {
  Option::add("homeFolder", "$HOME/.teditor", "Path to editor home folder",
              Option::Type::String);
  Option::add("quitAfterLoad", "NO",
              "Quit after parsing cmdline args and loading input files",
              Option::Type::Boolean);
  Option::add("rc", "<homeFolder>/rcfile", "Path to rc file",
              Option::Type::String);
}

std::vector<FileInfo> parseArgs(int argc, char** argv) {
  registerAllOptions();
  std::vector<FileInfo> files;
  for(int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-dump")) {
      ++i;
      ASSERT(i < argc, "'-dump' option expects an argument!");
      Option::dumpAll(argv[i]);
    } else {
      ASSERT(argv[i][0] != '-', "Invalid arg passed! '%s'", argv[i]);
      files.push_back(readFileInfo(argv[i]));
    }
  }
  makeDir(Option::get("homeFolder").getStr());
  return files;
}

}  // namespace teditor
