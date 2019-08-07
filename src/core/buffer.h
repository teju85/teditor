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


namespace teditor {

/** holder for lines removed during keep-lines */
struct RemovedLine {
  /** the removed line */
  std::string str;
  /** from where it was removed */
  Pos2d<int> pos;
};
/** vector of removed lines */
typedef std::vector<RemovedLine> RemovedLines;


class Editor;


/** Class for representing text files as a vector of lines (aka Buffer) */
class Buffer {
public:
  Buffer(const std::string& name="");
  virtual ~Buffer() {}

  /**
   * @defgroup BufferEdit Various of editing chars in the buffer
   * @{
   */
  /** insert a character at all current cursor locations */
  virtual void insert(char c);
  /**
   * insert strings, each at a given cursor location. For this to work, one
   * should pass as many strings as there are cursors!
   */
  void insert(const Strings& strs);
  /** insert a string at all current cursor locations */ 
  virtual void insert(const std::string& buf);
  /**
   * @brief main remove method
   * @param removeCurrent whether to remove the current char over the cursor
   */
  void remove(bool removeCurrent=false);
  /** remove a region and return its contents to be copied to clipboard */
  Strings removeAndCopy();
  /**
   * @brief removes regions between start and end
   * @param start list of region starts
   * @param end list of region ends
   * @return list of deleted regions
   * @todo make this private after updating unittests accordingly
   */
  Strings removeRegion(const Positions& start, const Positions& end);
  /** kills lines at current cursor location onwards and returns them */
  Strings killLine();
  /** sorts the lines in the regions */
  void sortRegions();
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

  /** find matching paren at the current location */
  void matchCurrentParen();

  /**
   * @brief resize the buffer in case of window size change
   * @param start screen start location
   * @param dim screen dimension
   */
  virtual void resize(const Pos2d<int>& start, const Pos2d<int>& dim);

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
  Pos2d<int> buffer2screen(const Pos2d<int>& loc) const;
  Pos2d<int> screen2buffer(const Pos2d<int>& loc) const;
  /** @} */

  /** returns character at a given buffer location */
  char charAt(const Pos2d<int>& pos) const;

  /**
   * @defgroup Draw Functions to draw parts of the buffer
   * @{
   */
  virtual void draw(Editor& ed, int currId);
  void drawCursor(Editor& ed, const AttrColor& bg);
  /** @} */

  /**
   * @defgroup CursorMovements All methods for moving cursor
   * @{
   */
  /** move to begining of line */
  void startOfLine();
  /** move to end of line */
  void endOfLine();
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
  void gotoLine(int lineNum);
  /** @} */

  /**
   * @defgroup CursorOps Cursor related operations
   * @{
   */
  /** add a cursor from back */
  void addCursorFromBack(const Pos2di& pos);
  /** add a cursor from front */
  void addCursorFromFront(const Pos2di& pos);
  /** clear all cursors except the first one */
  void clearAllCursorsButFirst();
  /** cursor count */
  int cursorCount() const { return (int)locs.size(); }
  /** checks if there are any cursors on the given line */
  bool hasCursorOn(int line) const;
  /** save the current state of all cursors */
  Positions saveCursors() const { return copyCursors(locs); }
  /** restore the state of all cursors to the given one */
  void restoreCursors(const Positions& pos);
  const Pos2di& cursorAt(int i) const { return locs[i]; }
  const Positions& cursors() const { return locs; }
  /** @} */

  /** length of a given line in this buffer */
  int lengthOf(int i) const { return lines[i].length(); }

  void indent();

  virtual int totalLinesNeeded() const;
  void lineUp();
  virtual void lineDown();
  void lineReset() { startLine = 0; }
  void lineEnd();
  virtual bool save(const std::string& fName="");
  const std::string& bufferName() const { return buffName; }
  const std::string& getFileName() const { return fileName; }
  const std::string& pwd() const { return dirName; }
  bool isRO() const { return readOnly; }
  bool isModified() const { return modified; }
  virtual int getMinStartLoc() const { return 0; }
  std::string dirModeGetFileAtLine(int line);
  int start() const { return startLine; }

  /**
   * @defgroup RegionOps Operations with regions
   * @{
   */
  /** checks if there's an active region */
  bool isRegionActive() const { return !regions.empty(); }
  /**
   * Return the string that represents the currently highlighted region. In
   * case of multiple-cursors, this will return one such string for each
   * region
   */
  Strings regionAsStr() const;
  /** start a mark (or region) */
  void startRegion() { regions.enable(locs); }
  /** stop the currently active mark (or region) */
  void stopRegion() { regions.clear(); }
  const Regions& getRegions() const { return regions; }
  /** @} */

  void reload();
  const Positions& getRegionLocs() const { return regions.getLocs(); }
  const AttrColor& getColor(const std::string& name) const;
  const std::string& getWord() const { return mode->word(); }
  const std::string& modeName() const { return mode->name(); }
  void makeReadOnly();

protected:
  /** the operation type */
  enum OpType {
    /** insertion operation */
    OpInsert = 0,
    /** backspace operation */
    OpDelete,
  };


  /**
   * @brief The state before/after applying insertion/deletion operations on the
   * Buffer object
   */
  struct OpData {
    /** from where the operation started */
    Positions before;
    /** till where the operation was performed */
    Positions after;
    /** characters that were inserted/deleted in the above range */
    Strings strs;
    /** type of operation */
    OpType type;
  }; // end class OpData


  /** the stack for undo/redo operations */
  typedef std::stack<OpData> OpStack;


  Pos2d<int> screenStart, screenDim;
  std::vector<Line> lines;
  int startLine;
  bool modified, readOnly;
  std::string buffName, fileName, dirName;
  Regions regions;
  ///@todo: support applying multiple modes
  ModePtr mode;
  /** cursor(s) */
  Positions locs;
  /** stack of operations for undo */
  OpStack undoStack;
  /** stack of operations for redo */
  OpStack redoStack;


  void addLine() { lines.push_back(Line()); }
  void resetBufferState(int line, const std::string& file);
  KeyCmdMap& getKeyCmdMap() { return mode->getKeyCmdMap(); }
  void loadFile(const std::string& file, int line);
  void loadDir(const std::string& dir);
  std::string removeFrom(const Pos2d<int>& start, const Pos2d<int>& end);
  Pos2d<int> matchCurrentParen(int i, bool& isOpen);
  int dirModeFileOffset() const { return 24; }
  Positions copyCursors(const Positions& pos) const;


  /**
   * @defgroup DrawInl Internal draw operations
   * @{
   */
  void drawStatusBar(Editor& ed, int currId);
  int drawLine(int y, const std::string& line, Editor& ed, int lineNum,
               const AttrColor& fg, const AttrColor& bg);
  /** @} */

  /**
   * @defgroup BufferOpsImpl Internal buffer edit operations
   * @{
   */
  /** insert a character at the given cursor count */
  void insert(char c, size_t i);
  /** for deleting a char using backspace */
  Strings removeChar();
  /** removes chars at current cursor locations and returns them */
  Strings removeCurrentChar();
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
  std::string regionAsStr(const Pos2di& start, const Pos2di& end) const;


  /**
   * @defgroup CursorOps Internal cursor operation details
   * @{
   */
  /** remove cursors that are on the same location */
  void removeDuplicateCursors();
  /** helper method to apply a function on a given cursor */
  template <typename Lambda>
  void forEachCursor(Lambda op) {
    size_t idx = 0;
    for(auto& cu : locs) {
      op(cu, idx);
      ++idx;
    }
    removeDuplicateCursors();
  }
  /** helper method to apply a function on cursors from a given index */
  template <typename Lambda>
  void forEachCursorFrom(Lambda op, int i) {
    int len = cursorCount();
    for(int j=i;j<len;++j) op(locs[j], j);
  }
  /** find whether there's a cursor already at the input location */
  bool findCursor(const Pos2di& pos) const;
  /**
   * move right all cursors from the input location onwards, which are on the
   * same line
   */
  void moveRightCursorsOnSameLine(int i);
  /**
   * move left all cursors from the input location onwards, which are on the
   * same line
   */
  void moveLeftCursorsOnSameLine(int i);
  /** move down all cursors from the input location onwards */
  void moveDownAllNextCursors(int i);
  /** move up all cursors from the input location onwards */
  void moveUpAllNextCursors(int i);
  /** @} */

  friend class Editor;
};


/** a list of buffers */
class Buffers : public std::vector<Buffer*> {
public:
  ~Buffers();

  /** create a new buffer with a unique name and push it to the end */
  Buffer* push_back(const std::string& name);

  void clear();
  void push_back(Buffer* buf);

  /** list of buffer names, in the order they are found in this object */
  Strings namesList() const;

  /** erase buffer at the given index */
  void erase(int idx);

private:
  std::set<std::string> buffNames;

  std::string uniquify(const std::string& name) const;
};

}; // end namespace teditor
