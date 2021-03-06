#pragma once

#include "buffer.h"
#include "utils.h"


namespace teditor {

/** Functor to filter input string against a list of choices */
typedef bool(*ChoicesFilter)(const std::string&, const std::string&);


class CmdMsgBar;

class Choices {
public:
  Choices(ChoicesFilter cf): filter(cf), choiceIdx(-1) {}
  virtual ~Choices() {}
  virtual const std::string& at(int idx) const = 0;
  virtual int size() const = 0;
  virtual bool updateChoices(const std::string& str) { return false; }
  virtual std::string getFinalStr(int idx, const std::string& str) const = 0;
  virtual void updateMainBuffer(CmdMsgBar& cmBar) {}
  virtual void resetLocations(CmdMsgBar& cmBar);
  bool match(const std::string& line, const std::string& str) const;
  bool match(int idx, const std::string& str) const;
  void setChoiceIdx(int idx) { choiceIdx = idx; }
  int getChoiceIdx() const { return choiceIdx; }

private:
  ChoicesFilter filter;
  // only used to communicate the final choice of the user to the extensions
  // that create these choice objects!
  int choiceIdx;
};


class StringChoices: public Choices {
public:
  StringChoices(const Strings& arr, ChoicesFilter cf=strFind);
  const std::string& at(int idx) const { return options[idx]; }
  std::string getFinalStr(int idx, const std::string& str) const;
  int size() const { return (int)options.size(); }

protected:
  Strings options;
};


class CmdMsgBar: public Buffer {
public:
  CmdMsgBar();
  void insert(char c) override;
  void insert(const std::string& str) override;
  void draw(Editor& ed, const Window& win) override;
  int drawLine(int y, const std::string& line, Editor& ed, int lineNum,
               const Window& win) override;
  void load(const std::string& file, int line=0) override {}
  bool save(const std::string& fName="") override { return false; }
  void clear() override;
  void setMinLoc(int loc) { minLoc = loc; }
  int getMinStartLoc() const override { return minLoc; }
  int totalLinesNeeded(const Point& dim) const override;
  void setChoices(Choices* ch) { choices = ch; }
  void clearChoices();
  bool usingChoices() const { return choices != nullptr; }
  std::string getStr() const { return lines[0].get().substr(minLoc); }
  std::string getFinalChoice() const;
  void down();
  void up();
  void updateChoices();
  int getOptLoc() const { return optLoc; }
  void setOptLoc(int loc) { optLoc = loc; }
  void lineUp(const Point& dim) override;
  void lineDown() override;
  void setStartLine(int line) { startLine = line; }

private:
  /** useful during prompts, so as to not cross into the message itself! */
  int minLoc;
  /** external choices vector (usually options) that need to be rendered */
  Choices* choices;
  /** currently selected option */
  int optLoc;

  int linesNeeded(const std::string& str, int wid) const;
};

} // end namespace teditor
