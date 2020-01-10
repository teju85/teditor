#pragma once

#include "../base/text.h"
#include <unordered_map>
#include <string>
#include "number.h"
#include "core/utils.h"
#include "core/editor.h"
#include "core/parser/lexer.h"

namespace teditor {
namespace calc {

/** calc mode */
class CalcMode: public text::TextMode {
public:
  CalcMode();
  ~CalcMode();

  static Mode* create() { return new CalcMode; }
  static bool modeCheck(const std::string& file) { return file == "*calc"; }

  bool isPromptLine(Buffer& buf) const;
  bool isOnPrompt(Buffer& buf, bool includeTrailingSpace = false) const;
  bool isOnSeparator(Buffer& buf) const;

  void addVar(const std::string& name, const Num64& var) { vars[name] = var; }
  Num64& getVar(const std::string& name);

  void printHeader(Buffer& buf) const;
  void insertChar(Buffer& buf, char c, Editor& ed) const;
  void evaluate(Buffer& buf, Editor& ed);

private:
  struct Keys { static std::vector<KeyCmdPair> All; };
  struct Colors { static std::vector<NameColorPair> All; };

  std::unordered_map<std::string, Num64> vars;
  std::string prompt, lineSeparator;
  History cmds;
  parser::Lexer* lex;
};  // class CalcMode


/** Token types for Lexer */
enum TokenIds {
  Float,
  Int,
  Assignment,
  BrktOpen,
  BrktClose,
  Comma,
  SemiColon,
  Symbol,
  Operators,
  WhiteSpace,
};  // enum TokenIds

}  // namespace calc
}  // namespace teditor
