#pragma once

#include <string>
#include "colors.h"
#include <vector>
#include "utils.h"
#include "key_cmd_map.h"
#include "command.h"
#include "line.h"
#include "pcre.h"
#include "mode.h"
#include "pos2d.h"
#include <stack>
#include <vector>
#include <unordered_set>

namespace teditor {

/** holder for lines removed during keep-lines */
struct RemovedLine {
  /** the removed line */
  std::string str;
  /** from where it was removed */
  Point pos;
};
/** list of removed lines */
typedef std::vector<RemovedLine> RemovedLines;


class Editor;
class Window;


/** Class for representing text files as a vector of lines (aka Buffer) */
class Buffer {
public:
  Buffer(const std::string& name="", bool noUndoRedo=false);
  virtual ~Buffer() {}

  /**
   * @defgroup BufferEdit Various of editing chars in the buffer
   * @{
   */
  /** insert a character at the current cursor location */
  virtual void insert(char c);
  /** insert a string at the current cursor location */ 
  virtual void insert(const std::string& str);
  /**
   * @brief main remove method
   * @param removeCurrent whether to remove the current char over the cursor
   */
  void remove(bool removeCurrent=false);
  /** remove a region and return its contents to be copied to clipboard */
  std::string removeAndCopy();
  /**
   * @brief removes region between start and end
   * @param start start of the region
   * @param end end of the region
   * @return string present in the deleted region
   * @todo make this private after updating unittests accordingly
   */
  std::string removeRegion(const Point& start, const Point& end);
  /** kills lines at current cursor location onwards */
  std::string killLine(bool pushToStack=true);
  /** sorts the lines in the region */
  void sortRegion();
  /**
   * @brief Keep/Remove lines that match the input regex.
   * @param pc the regular expression that needs to be matched
   * @param keep whether to keep the matching lines or remove them
   * @return Returns all those that don't match (or match)
   */
  RemovedLines keepRemoveLines(Pcre& pc, bool keep);
  /** add the previously removed lines back into the buffer */
  void addLines(const RemovedLines& rlines);
  /** clear buffer contents */
  virtual void clear();
  /** @} */

  /**
   * @defgroup UndoRedoStack Undo/Redo operations on the buffer
   * @{
   */
  /**
   * @brief undo the previous operation
   * @return false if there's nothing to undo, else true */
  bool undo();
  /**
   * @brief redo the previously undid operation.
   * @return false if there's nothing to redo, else true
   */
  bool redo();
  /** @} */

  /**
   * @brief find matching paren at the current location
   * @return true if open-paren was matched, else false
   */
  bool matchCurrentParen();

  /** Load a file/dir into the buffer and optionally jump to a line */
  virtual void load(const std::string& file, int line=0);

  /** number of lines in this buffer */
  int length() const { return (int)lines.size(); }

  /**
   * @defgroup Accessor Accessing individual lines
   * @{
   */
  Line& at(int idx) { return lines[idx]; }
  const Line& at(int idx) const { return lines[idx]; }
  /** @} */

  /**
   * @defgroup Coordinates convert buffer co-ords to screen and vice versa
   * @{
   */
  Point buffer2screen(const Point& loc, const Point& start,
                      const Point& dim) const;
  Point screen2buffer(const Point& loc, const Point& start,
                       const Point& dim) const;
  /** @} */

  /** returns character at a given buffer location */
  char charAt(const Point& pos) const;

  /**
   * @defgroup Draw Functions to draw parts of the buffer
   * @{
   */
  virtual void draw(Editor& ed, const Window& win);
  void drawPoint(Editor& ed, const AttrColor& bg, const Window& win);
  /** @} */

  /**
   * @defgroup PointMovements All methods for moving cursor
   * @{
   */
  /** move to begining of line */
  void startOfLine() { cu.x = getMinStartLoc(); }
  /** move to end of line */
  void endOfLine() { cu.x = lengthOf(cu.y); }
  /** move left */
  void left();
  /** move right */
  void right();
  /** move down */
  void down();
  /** move up */
  void up();
  /** go to beginning of the buffer */
  void begin();
  /** go to end of the buffer */
  void end();
  /** scroll down a page */
  void pageDown(int ijump);
  /** scroll up a page */
  void pageUp(int ijump);
  /** jump to start of next paragraph */
  void nextPara();
  /** jump to start of previous paragraph */
  void previousPara();
  /** jump to next word */
  void nextWord();
  /** jump to previous word */
  void previousWord();
  /** goto the specified line number */
  void gotoLine(int lineNum, const Point& dim);
  /** @} */

  /**
   * @defgroup PointOps Point related operations
   * @{
   */
  /** checks if there are any cursors on the given line */
  bool hasPointOn(int line) const { return line == cu.y; }
  const Point& getPoint() const { return cu; }
  void setPoint(const Point& p) { cu = p; }
  /** @} */

  /**
   * @defgroup RegionOps Operations with regions
   * @{
   */
  /** checks if there's an active region */
  bool isRegionActive() const { return region != Point(-1, -1); }
  /** Returns the string that represents the currently highlighted region */
  std::string regionAsStr() const;
  /** start a mark (or region) */
  void startRegion() { region = cu; }
  /** stop the currently active mark (or region) */
  void stopRegion() { region = Point(-1, -1); }
  const Point& getRegion() const { return region; }
  /** @} */

  /** length of a given line in this buffer */
  int lengthOf(int i) const { return lines[i].length(); }

  /** indent the current line */
  void indent();

  /** number of lines needed to draw the currrent buffer in this window */
  virtual int totalLinesNeeded(const Point& dim) const;

  virtual void lineUp(const Point& dim);
  virtual void lineDown();
  void lineReset() { startLine = 0; }
  void lineEnd(const Point& start, const Point& dim);
  virtual bool save(const std::string& fName="");
  const std::string& bufferName() const { return buffName; }
  const std::string& getFileName() const { return fileName; }
  const std::string& pwd() const { return dirName; }
  bool isRO() const { return readOnly; }
  bool isModified() const { return modified; }
  virtual int getMinStartLoc() const { return 0; }
  std::string dirModeGetFileAtLine(int line);

  void reload();
  const AttrColor& getColor(const std::string& name) const;
  const std::string& getWord() const { return mode->word(); }
  const std::string& modeName() const { return mode->name(); }
  void makeReadOnly();
  void setMode(ModePtr m) { mode = m; }

protected:
  /** the operation type */
  enum OpType {
    /** insertion operation */
    OpInsert = 0,
    /** backspace operation */
    OpDelete,
    /** deleting the rest of the line */
    OpKillLine
  };


  /**
   * @brief The state before/after applying insertion/deletion operations on the
   * Buffer object
   */
  struct OpData {
    /** from where the operation started */
    Point before;
    /** till where the operation was performed */
    Point after;
    /** characters that were inserted/deleted in the above range */
    std::string str;
    /** type of operation */
    OpType type;
  }; // end class OpData


  /** the stack for undo/redo operations */
  typedef std::stack<OpData> OpStack;


  std::vector<Line> lines;
  int startLine;
  bool modified, readOnly;
  std::string buffName, fileName, dirName;
  /** start of a region */
  Point region;
  ///@todo: support applying multiple modes
  ModePtr mode;
  /** cursor */
  Point cu;
  /** stack of operations for undo */
  OpStack undoStack;
  /** stack of operations for redo */
  OpStack redoStack;
  /** whether to disable undo/redo stack for this buffer */
  bool disableStack;


  void insertImpl(char c);
  void addLine() { lines.push_back(Line()); }
  void resetBufferState(int line, const std::string& file);
  KeyCmdMap& getKeyCmdMap() { return mode->getKeyCmdMap(); }
  void loadFile(const std::string& file, int line);
  void loadDir(const std::string& dir);
  std::string removeFrom(const Point& start, const Point& end);
  Point matchCurrentParen(bool& isOpen);
  int dirModeFileOffset() const { return 24; }


  /**
   * @defgroup DrawInl Internal draw operations
   * @{
   */
  void drawStatusBar(Editor& ed, const Window& win);
  virtual int drawLine(int y, const std::string& line, Editor& ed, int lineNum,
                       const Window &win);
  /** @} */

  /**
   * @defgroup BufferOpsImpl Internal buffer edit operations
   * @{
   */
  /** for deleting a char using backspace */
  std::string removeChar();
  /** removes chars at current cursor locations and returns them */
  std::string removeCurrentChar();
  /** @} */


  /**
   * @defgroup UndoRedo Internal operations associated with undo/redo stack
   * @{
   */
  /**
   * @brief Insert characters into the buffer
   * @param op the info on what, how and where to insert
   * @param pushToStack whether to push this op into the 'undoStack'
   */
  void applyInsertOp(OpData& op, bool pushToStack=true);
  /**
   * @brief Delete characters/regions from the buffer
   * @param op the info on what, how and where to delete from
   */
  void applyDeleteOp(OpData& op);
  /**
   * pushing a new op onto the undo stack. It has the side-effect of clearing
   * the redo stack so far accumulated!
   */
  void pushNewOp(OpData& op);
  /** clear the input stack (esp useful while clearing redo stack) */
  void clearStack(OpStack& st);
  /** @} */


  /** helper method to return the string in the given region */
  std::string regionAsStr(const Point& start, const Point& end) const;

  /** find whether there's a cursor already at the input location */
  bool findPoint(const Point& pos) const { return cu == pos; }

  friend class Editor;
};


/** a list of buffers */
class Buffers : public std::vector<Buffer*> {
public:
  ~Buffers();

  /** create a new buffer with a unique name and push it to the end */
  Buffer* push_back(const std::string& name);
  void push_back(Buffer* buf);

  void clear();
  void erase(int idx);

  /** list of buffer names, in the order they are found in this object */
  Strings namesList() const;


private:
  std::unordered_set<std::string> buffNames;

  std::string uniquify(const std::string& name) const;
};

}; // end namespace teditor
