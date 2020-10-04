#pragma once

namespace teditor {
namespace todo {

/** todo file parser! */
class Parser {
public:
  Parser(const std::string& f): file(f) { reload(); }

  void reload() { parse(file); }

private:
  std::string file;

  void parse(const std::string& f);
};

} // end namespace todo
} // end namespace teditor
