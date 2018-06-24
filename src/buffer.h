#pragma once

#include <string>
#include "colors.h"
#include <vector>
#include "utils.h"
#include "key_cmd_map.h"
#include "command.h"
#include "line.h"
#include "pcre.h"
#include "cursor.h"
#include "gtest/gtest.h"
#include "mode.h"


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
    virtual void insert(const char* buf);
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

    ///@todo unit-test this
    /**
     * @brief Keep/Remove lines that match the input regex.
     * @param pc the regular expression that needs to be matched
     * @param keep whether to keep the matching lines or remove them
     * @return Returns all those that don't match (or match)
     */
    RemovedLines keepRemoveLines(Pcre& pc, bool keep);

    ///@todo unit-test this
    /** add the previously removed lines back into the buffer */
    void addLines(const RemovedLines& rlines);

    ///@todo unit-test this
    /** find matching paren at the current location */
    void matchCurrentParen();
    ///@todo unit-test this
    Pos2d<int> matchCurrentParen(int i, bool& isOpen);

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

    virtual void drawBuffer(Editor& ed);
    void drawCursor(Editor& ed, const std::string& bg);
    virtual void drawStatusBar(Editor& ed);
    int drawLine(int y, const std::string& line, Editor& ed, int lineNum,
                 const std::string& fg, const std::string& bg);

    Pos2d<int> buffer2screen(const Pos2d<int>& loc) const;
    Cursor& getCursor() { return cursor; }
    const Cursor& getCursor() const { return cursor; }
    virtual int totalLinesNeeded() const;
    char charAt(const Pos2d<int>& pos) const;
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
    int dirModeFileOffset() const { return 24; }
    Strings regionAsStr() const;
    virtual void clear();
    void reload();
    void addCommand(CmdPtr c);
    void undo();
    void redo();
    const Positions& getRegionLocs() const { return regions.getLocs(); }
    const AttrColor& getColor(const std::string& name) const;
    int verticalJump(float jump) const { return (int)(jump * screenDim.y); }
    const std::string& getWord() const { return mode.word; }

protected:
    Pos2d<int> screenStart, screenDim;
    std::vector<Line> lines;
    int startLine;
    Cursor cursor;
    bool modified, readOnly;
    std::string buffName, fileName, dirName;
    Regions regions;
    bool regionActive;
    std::vector<CmdPtr> cmds;
    int topCmd;
    Mode mode;

    void insertLine(int i);
    void insert(char c, int i);
    void addLine() { lines.push_back(Line()); }
    void resetBufferState(int line, const std::string& file);
    void enableRegions();
    void disableRegions();
    KeyCmdMap& getKeyCmdMap() { return mode.kcMap; }
    void loadFile(const std::string& file, int line);
    void loadDir(const std::string& dir);
    std::string removeFrom(const Pos2d<int>& start, const Pos2d<int>& end);

    friend class Editor;
    FRIEND_TEST(Buffer, SortRegionsEmptyLine);
    FRIEND_TEST(Buffer, SortRegions);
};

}; // end namespace teditor
