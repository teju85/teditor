#include <fstream>
#include <unordered_map>
#include "option.h"
#include <memory>


namespace teditor {

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

}  // namespace teditor
