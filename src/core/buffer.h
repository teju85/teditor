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
    virtual ~Buffer();

    /**
     * @defgroup Insertion Various of adding characters into the buffer
     * @{
     */
    virtual void insert(char c);
    void insert(const Strings& strs);
    virtual void insert(const std::string& buf);
    /** @} */

    /**
     * @defgroup Deletion Various ways of deleting chars from the buffer
     * @{
     */
    /** for deleting a char using backspace */
    Strings remove();
    /**
     * @brief removes regions between start and end
     * @param start list of region starts
     * @param end list of region ends
     * @return list of deleted regions
     */
    Strings remove(const Positions& start, const Positions& end);
    /** removes chars at current cursor locations and returns them */
    Strings removeCurrent();
    /** kills lines at current cursor location onwards and returns them */
    Strings killLine();
    /** @} */

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

    /** find matching paren at the current location */
    void matchCurrentParen();

    /** goto the specified line number */
    void gotoLine(int lineNum);

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
    virtual void drawBuffer(Editor& ed);
    void drawCursor(Editor& ed, const std::string& bg);
    virtual void drawStatusBar(Editor& ed);
    int drawLine(int y, const std::string& line, Editor& ed, int lineNum,
                 const std::string& fg, const std::string& bg);
    /** @} */

    /** clear buffer contents */
    virtual void clear();

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
    void pageDown(float jump);
    /** scroll up a page */
    void pageUp(float jump);
    /** jump to start of next paragraph */
    void nextPara();
    /** jump to start of previous paragraph */
    void previousPara();
    /** jump to next word */
    void nextWord();
    /** jump to previous word */
    void previousWord();
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
    Positions saveCursors() const;
    /** restore the state of all cursors to the given one */
    void restoreCursors(const Positions& pos);
    /** cursor at i'th index */
    const Pos2di& cursorAt(int i) const { return locs[i]; }
    /** @} */

    /** length of a given line in this buffer */
    int lengthOf(int i) const { return lines[i].length(); }

    void indent();

    virtual int totalLinesNeeded() const;
    void lineUp();
    virtual void lineDown();
    void lineReset() { startLine = 0; }
    void lineEnd();
    virtual void save();
    const std::string& bufferName() const { return buffName; }
    const std::string& getFileName() const { return fileName; }
    const std::string& pwd() const { return dirName; }
    bool isRO() const { return readOnly; }
    bool isModified() const { return modified; }
    bool isRegionActive() const { return regionActive; }
    virtual int getMinStartLoc() const { return 0; }
    std::string dirModeGetFileAtLine(int line);

    /**
     * Return the string that represents the currently highlighted region. In
     * case of multiple-cursors, this will return one such string for each
     * region
     */
    Strings regionAsStr() const;

    void reload();
    void addCommand(CmdPtr c);
    void undoCmd();
    void redoCmd();
    const Positions& getRegionLocs() const { return regions.getLocs(); }
    const AttrColor& getColor(const std::string& name) const;
    int verticalJump(float jump) const { return (int)(jump * screenDim.y); }
    const std::string& getWord() const { return mode->word(); }
    void enableRegions();
    void disableRegions();
    const std::string& modeName() const { return mode->name(); }

protected:
    /** the operation type */
    enum OpType {
        /** insertion operation */
        OpInsert = 0,
        /** backspace operation */
        OpDelete
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
        Strings chars;
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
    bool regionActive;
    std::vector<CmdPtr> cmds;
    int topCmd;
    ///@todo: support applying multiple modes
    ModePtr mode;
    /** cursor(s) */
    Positions locs;
    /** stack of operations for undo */
    OpStack undoStack;
    /** stack of operations for redo */
    OpStack redoStack;

    void insertLine(int i);
    void insert(char c, int i);
    void addLine() { lines.push_back(Line()); }
    void resetBufferState(int line, const std::string& file);
    KeyCmdMap& getKeyCmdMap() { return mode->getKeyCmdMap(); }
    void loadFile(const std::string& file, int line);
    void loadDir(const std::string& dir);
    std::string removeFrom(const Pos2d<int>& start, const Pos2d<int>& end);
    Pos2d<int> matchCurrentParen(int i, bool& isOpen);
    int dirModeFileOffset() const { return 24; }

    /** clear the input stack (esp useful while clearing redo stack) */
    void clearStack(OpStack& st);

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

}; // end namespace teditor
