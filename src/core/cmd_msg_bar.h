#pragma once

#include "buffer.h"
#include "utils.h"


namespace teditor {

/** Functor to filter input string against a list of choices */
typedef bool(*ChoicesFilter)(const std::string&, const std::string&);


class CmdMsgBar;

class Choices {
public:
  Choices(ChoicesFilter cf): filter(cf), optLoc(-1) {}
  virtual ~Choices() {}
  virtual const std::string& at(size_t idx) const = 0;
  virtual size_t size() const = 0;
  virtual bool updateChoices(const std::string& str) { return false; }
  virtual std::string getFinalStr(size_t idx, const std::string& str) const = 0;
  virtual void updateMainBuffer(CmdMsgBar& cmBar) {}
  bool match(const std::string& line, const std::string& str) const;
  bool match(size_t idx, const std::string& str) const;
  void setIdx(size_t idx) { optLoc = idx; }
  size_t getIdx() const { return optLoc; }

private:
  ChoicesFilter filter;
  size_t optLoc;
};


class StringChoices: public Choices {
public:
  StringChoices(const Strings& arr, ChoicesFilter cf=strFind);
  const std::string& at(size_t idx) const { return options[idx]; }
  std::string getFinalStr(size_t idx, const std::string& str) const;
  size_t size() const { return options.size(); }

protected:
  Strings options;
};


class CmdMsgBar: public Buffer {
public:
  CmdMsgBar();
  void insert(char c) override;
  void insert(const std::string& str) override;
  void draw(Editor& ed, const Window& win) override;
  size_t drawLine(size_t y, const std::string& line, Editor& ed, size_t lineNum,
                  const Window& win) override;
  void load(const std::string& file, size_t line = 0) override {}
  bool save(const std::string& fName="") override { return false; }
  void clear() override;
  void setMinLoc(size_t loc) { minLoc = loc; }
  size_t getMinStartLoc() const override { return minLoc; }
  size_t totalLinesNeeded(const Point& dim) const override;
  void setChoices(Choices* ch) { choices = ch; }
  void clearChoices();
  bool usingChoices() const { return choices != nullptr; }
  std::string getStr() const { return lines[0].get().substr(minLoc); }
  std::string getFinalChoice() const;
  void down();
  void up();
  void updateChoices();
  size_t getOptLoc() const { return optLoc; }
  void lineUp(const Point& dim) override;
  void lineDown() override;

private:
  /** useful during prompts, so as to not cross into the message itself! */
  size_t minLoc;
  /** external choices vector (usually options) that need to be rendered */
  Choices* choices;
  /** currently selected option */
  size_t optLoc;

  size_t linesNeeded(const std::string& str, size_t wid) const;
};

} // end namespace teditor
