#pragma once

#include "objects.h"

namespace teditor {
namespace todo {

/** todo file parser! */
class Parser {
public:
  Parser(const std::string& f): file(f) { reload(); }

  void reload() { parse(file); }

  const CalendarItems& items() const { return items_; }

private:
  std::string file;
  CalendarItem curr;
  CalendarItems items_;

  void parse(const std::string& f);
};

} // end namespace todo
} // end namespace teditor
