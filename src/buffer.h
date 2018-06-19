#pragma once

#include <string>
#include "colors.h"
#include <vector>
#include <string.h>
#include "utils.h"
#include "key_cmd_map.h"
#include "command.h"
#include "pcre.h"


namespace teditor {

class ByteBuffer: public std::string {
public:
    ByteBuffer(int cap);
    void reserve(int cap);
    void append(const char* data, int len);
    void puts(const char* data);
    void flush(int fd);
};


// a Unicode character
typedef uint32_t Chr;

// a conceptual entity on the terminal screen
struct Cell {
    Chr ch;
    AttrColor fg, bg;

    void set(Chr c, AttrColor f, AttrColor b);
    void set(Chr c, color_t f, color_t b);
    void copy(const Cell& from);
    int width() const;
};

bool operator==(const Cell& a, const Cell& b);


class CellBuffer {
public:
    CellBuffer(int w=0, int h=0);
    void clear(AttrColor fg, AttrColor bg);
    void resize(int w, int h);
    const Cell& at(int x, int y) const { return cells[y*width+x]; }
    Cell& at(int x, int y) { return cells[y*width+x]; }
    unsigned w() const { return (unsigned)width; }
    unsigned h() const { return (unsigned)height; }

private:
    int width, height;
    std::vector<Cell> cells;
};


class Editor;
class Regions;
class Cursor;

///@todo: support for unicode
class Line {
public:
    void append(char c) { line.push_back(c); }
    void append(const char* c) { line += c; }
    void append(const std::string& str) { line += str; }
    void prepend(char c) { insert(c, 0); }
    void prepend(const char* c) { insert(c, 0); }
    void insert(char c, int idx);
    void insert(const char* c, int idx);
    void insert(const std::string& str, int idx) { insert(str.c_str(), idx); }
    std::string erase(int idx, int len=1);
    int numLinesNeeded(int wid) const;
    bool empty() const { return length() == 0; }
    int length() const { return (int)line.length(); }
    const std::string& get() const { return line; }
    char at(int idx) const { return line[idx]; }
    void clear() { line.clear(); }
    int findFirstNotOf(const std::string& str, int pos) const;
    int findLastNotOf(const std::string& str, int pos) const;

private:
    std::string line;
};


typedef std::vector<Pos2d<int> > Positions;


class MultiLine;

class Cursor {
public:
    Cursor();
    void home(MultiLine* ml);
    void lineEnd(MultiLine* ml);
    void left(MultiLine* ml);
    void right(MultiLine* ml);
    void down(MultiLine* ml);
    void up(MultiLine* ml);
    void reset(MultiLine* ml);
    void end(MultiLine* ml);
    void pageDown(MultiLine* ml, float jump);
    void pageUp(MultiLine* ml, float jump);
    void nextPara(MultiLine* ml);
    void previousPara(MultiLine* ml);
    void nextWord(MultiLine* ml);
    void previousWord(MultiLine* ml);
    void addBack(int cx, int cy);
    void addBack(Pos2d<int>& pos);
    void addFront(int cx, int cy);
    void addFront(Pos2d<int>& pos);
    void remove(int id);
    bool isHidden(int id) const;
    bool isHidden(int cx, int cy) const;
    int count() const { return locs.size(); }
    const Pos2d<int>& at(int id) const { return locs[id]; }
    Pos2d<int>& at(int id) { return locs[id]; }
    Positions saveExcursion() const;
    void restoreExcursion(const Positions& pos);
    /** clear all cursors except the first (useful for multiple-cursors mode) */
    void clearAllButFirst();

    static const int Hidden;

private:
    Positions locs;

    void removeDuplicates();
    void moveRightCursorsOnSameLine(int i);
    void moveDownAllNextCursors(int i);
    void moveUpAllNextCursors(int i);
    void moveLeftCursorsOnSameLine(int i);

    bool findCursor(const Pos2d<int>& pos) const;
    const Positions& getLocs() const { return locs; }

    friend class MultiLine;
};


class Regions {
public:
    Regions(): locs() {}
    void enable(const Positions& p);
    void clear() { locs.clear(); }
    bool isInside(int y, int x, const Cursor& cu) const;
    bool isInside(int y, int x, const Cursor& cu, int i) const;
    const Pos2d<int>& at(int idx) const { return locs[idx]; }
    int count() const { return (int)locs.size(); }
    const Positions& getLocs() const { return locs; }

private:
    Positions locs;
};


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

class MultiLine {
public:
    MultiLine(const std::string& name="");
    virtual ~MultiLine();

    void insertLine(int i);
    void insert(char c);
    void insert(char c, int i);
    void insert(const std::vector<std::string>& strs);
    void insert(const char* buf);
    std::vector<std::string> remove();
    /**
     * @brief removes regions between start and end
     * @param start list of region starts
     * @param end list of region ends
     * @return list of deleted regions
     */
    std::vector<std::string> remove(const Positions& start,
                                    const Positions& end);
    /** removes chars at current cursor locations and returns them */
    std::vector<std::string> removeCurrent();
    /** kills lines at current cursor location onwards and returns them */
    std::vector<std::string> killLine();

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
    Pos2d<int> matchCurrentParen(int i, bool& isOpen);

    /** goto the specified line number */
    void gotoLine(int lineNum);

    void resize(const Pos2d<int>& start, const Pos2d<int>& dim);
    void load(const std::string& file, int line=0);
    int length() const { return (int)lines.size(); }
    Line& at(int idx) { return lines[idx]; }
    const Line& at(int idx) const { return lines[idx]; }

    void drawBuffer(Editor& ed);
    void drawCursor(Editor& ed, const std::string& bg);
    void drawStatusBar(Editor& ed);
    int drawLine(int y, const std::string& line, Editor& ed, int lineNum,
                 const std::string& fg, const std::string& bg);

    Pos2d<int> buffer2screen(const Pos2d<int>& loc) const;
    Cursor& getCursor() { return cursor; }
    const Cursor& getCursor() const { return cursor; }
    void addLine() { lines.push_back(Line()); }
    int totalLinesNeeded() const;
    char charAt(const Pos2d<int>& pos) const;
    void lineUp();
    void lineDown();
    void lineReset() { startLine = 0; }
    void lineEnd();
    void save();
    const std::string& bufferName() const { return buffName; }
    const std::string& getFileName() const { return fileName; }
    const std::string& pwd() const { return dirName; }
    bool isRO() const { return readOnly; }
    bool isModified() const { return modified; }
    bool isRegionActive() const { return regionActive; }
    virtual int getMinStartLoc() const { return 0; }
    int dirModeFileOffset() const { return 24; }
    std::vector<std::string> regionAsStr() const;
    void clear();
    void reload();
    void addCommand(CmdPtr c);
    void undo();
    void redo();
    const Positions& getRegionLocs() const { return regions.getLocs(); }
    const AttrColor& getColor(const std::string& name) const;
    int verticalJump(float jump) const { return (int)(jump * screenDim.y); }
    const std::string& getWord() const { return word; }

protected:
    Pos2d<int> screenStart, screenDim;
    std::vector<Line> lines;
    int startLine;
    Cursor cursor;
    bool modified, readOnly;
    std::string buffName, fileName, dirName;
    Regions regions;
    bool regionActive;
    KeyCmdMap kcMap;
    ColorMap cMap;
    std::vector<CmdPtr> cmds;
    int topCmd;
    std::string word;

    void resetBufferState(int line, const std::string& file);
    void enableRegions();
    void disableRegions();
    KeyCmdMap& getKeyCmdMap() { return kcMap; }
    void loadFile(const std::string& file, int line);
    void loadDir(const std::string& dir);
    std::string removeFrom(const Pos2d<int>& start, const Pos2d<int>& end);

    friend class Editor;
};


class CmdMsgBar: public MultiLine {
public:
    CmdMsgBar();
    void resize(const Pos2d<int>& start, const Pos2d<int>& dim);
    void load(const std::string& file, int line=0) {}
    void insert(char c);
    void insert(const char* str);
    void drawBuffer(Editor& ed);
    void drawStatusBar(Editor& ed) {}
    void save() {}
    void clear();
    void setMinLoc(int loc) { minLoc = loc; }
    int getMinStartLoc() const override { return minLoc; }
    void setOptions(const std::vector<std::string>& opts) { options = opts; }
    void clearOptions() { options.clear(); }
    bool usingOptions() const { return !options.empty(); }

private:
    /** useful during prompts, so as to not cross into the message itself! */
    int minLoc;
    /** external vector (usually options) that need to be rendered */
    std::vector<std::string> options;
};

}; // end namespace teditor
