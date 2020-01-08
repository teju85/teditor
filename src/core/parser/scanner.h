#pragma once

#include <string>
#include "core/pos2d.h"

namespace teditor {
namespace parser {

/** Abstract base class for all scanners */
class Scanner {
 public:
  virtual ~Scanner() {}
  virtual void setStart(const Point& pt) = 0;
  virtual char next(Point& pt) = 0;
  virtual bool isEof() const = 0;
  virtual void rewind() = 0;
};  // class Scanner


class StringScanner : public Scanner {
 public:
  StringScanner(const std::string& s): str(s), currPos(0), startPos(0) {}
  void setStart(const Point& pt) override;
  char next(Point& pt) override;
  bool isEof() const override { return currPos >= str.size(); }
  void rewind() override;

 private:
  std::string str;
  size_t currPos;
  size_t startPos;
};  // class StringScanner

}  // namespace parser
}  // namespace teditor
