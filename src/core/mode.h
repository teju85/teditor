#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "utils.h"


namespace teditor {

class Buffer;
class KeyCmdMap;
class ColorMap;
class Mode;
class Pcre;


typedef Mode* (*ModeCreator)();
typedef std::shared_ptr<Mode> ModePtr;
typedef std::unordered_map<std::string,ModeCreator> ModeCreatorMap;

typedef bool (*InferMode)(const std::string&);
typedef std::unordered_map<std::string,InferMode> ModeInferMap;


/** Mode attached with a buffer */
class Mode {
public:
  /**
   * @brief ctor
   * @param n name of the mode
   * @param w list of chars that define a word in this mode
   */
  Mode(const std::string& n, const std::string& w): name_(n), word_(w) {}

  /** dtor */
  virtual ~Mode() {}

  /** mode's name */
  virtual const std::string& name() const { return name_; }

  /** list of chars that define a word */
  virtual const std::string& word() const { return word_; }

  /** function to compute indentation of the line in the input buffer */
  virtual int indent(Buffer& buf, int line) = 0;

  /** get key-cmd map for the buffer this mode applies to */
  virtual KeyCmdMap& getKeyCmdMap() = 0;

  /** get color map for the buffer this mode applies to */
  virtual ColorMap& getColorMap() = 0;

  /**
   * @brief helper to register a named mode
   * @param mode name of the mode (useful for later queries)
   * @param fptr functor that helps create the mode object
   * @param iptr functor to check if this mode applies to the input file
   */
  static void registerMode(const std::string& mode, ModeCreator fptr,
                           InferMode iptr);

  /**
   * @brief Helper to create mode object of the named mode
   * @param mode name of the mode
   * @return the mode object pointer
   */
  static ModePtr createMode(const std::string& mode);

  /**
   * @brief Helper to infer mode name from the file
   * @param file the file
   * @return the mode name
   */
  static std::string inferMode(const std::string& file);

private:
  /** mode name */
  std::string name_;
  /** list of chars that define a word */
  std::string word_;
};


/** Accessor function to the list of mode names */
Strings allModeNames();

/** infers mode from it's filename */
std::string inferMode(const std::string& file);


class RegisterMode {
public:
  RegisterMode(const std::string& mode, ModeCreator fptr, InferMode iptr) {
    Mode::registerMode(mode, fptr, iptr);
  }
};


#define REGISTER_MODE(Name, NameStr)                                    \
  RegisterMode mode ## Name(NameStr, Name::create, Name::modeCheck)

}; // end namespace teditor
