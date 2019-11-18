#pragma once

#include <stdlib.h>
#include <string>
#include "utils.h"
#include "file_utils.h"
#include <vector>

namespace teditor {

/**
 * @brief Class to define an option used in the editor
 */
class Option {
 public:
  /** All supported option types */
  enum Type {
    Boolean,
    Integer,
    Real,
    String,
    Char
  };  // enum Type

  /**
   * @brief Define a new option
   * @param n name of the option
   * @param v default value
   * @param hm help message for this option
   * @param t type of option
   */
  Option(const std::string& n, const std::string& v, const std::string& hm,
         Type t) : name(n), value(v), helpMsg(hm), type(t) {}

  /**
   * @defgroup ValueGetters Get underlying value
   * @{
   */
  bool getBool() const {
    ASSERT(type == Boolean, "Option '%s' is not Boolean!", name.c_str());
    return value == "YES";
  }

  int getInt() const {
    ASSERT(type == Integer, "Option '%s' is not Integer!", name.c_str());
    return str2num(value);
  }

  float getReal() const {
    ASSERT(type == Real, "Option '%s' is not Real!", name.c_str());
    return str2real(value);
  }

  std::string getStr() const;

  char getChar() const {
    ASSERT(type == Char, "Option '%s' is not Char!", name.c_str());
    return value[0];
  }
  /** @} */

  /**
   * @brief Define a new option
   * @param n name of the option
   * @param v default value
   * @param hm help message for this option
   * @param t type of option
   */
  static void add(const std::string& n, const std::string& v,
                  const std::string& hm, Type t);

  /** sets value for the given option */
  static void set(const std::string& n, const std::string& val);

  /** gets the named option */
  static Option& get(const std::string& n);

  /** dumps all options into the rcfile */
  static void dumpAll(const std::string& outfile);

 private:
  /** name of this option */
  std::string name;
  /** value of this option */
  std::string value;
  /** short help message with this option */
  std::string helpMsg;
  /** type of this option */
  Type type;

  void setVal(const std::string& val) { value = val; }

  static std::string type2str(Type t);
  static std::string expandOptions(const std::string& str);
};  // class Option


/** parse input commandline args */
std::vector<FileInfo> parseArgs(int argc, char** argv);

}  // namespace teditor
