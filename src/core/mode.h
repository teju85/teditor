#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "utils.h"
#include "colors.h"

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

  /** get color for the given line */
  virtual void getColorFor(AttrColor& fg, AttrColor& bg, int lineNum,
                           const Buffer& b, bool isHighlighted) = 0;

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

  struct Registrar {
    Registrar(const std::string& mode, ModeCreator fptr, InferMode iptr);
  };  // struct Registrar

private:
  /** mode name */
  std::string name_;
  /** list of chars that define a word */
  std::string word_;
};  // class Mode


/** Accessor function to the list of mode names */
Strings allModeNames();


#define REGISTER_MODE(Name, NameStr)                                    \
  Mode::Registrar mode ## Name(NameStr, Name::create, Name::modeCheck)

}; // end namespace teditor
