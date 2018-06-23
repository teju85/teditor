#pragma once

#include <string>
#include "colors.h"
#include <vector>
#include <string.h>
#include "utils.h"
#include "key_cmd_map.h"
#include "command.h"
#include "line.h"
#include "pcre.h"
#include "cursor.h"


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

class MultiLine {
public:
    MultiLine(const std::string& name="");
    virtual ~MultiLine();

    void insertLine(int i);
    virtual void insert(char c);
    void insert(char c, int i);
    void insert(const std::vector<std::string>& strs);
    virtual void insert(const char* buf);
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
    Pos2d<int> matchCurrentParen(int i, bool& isOpen);

    /** goto the specified line number */
    void gotoLine(int lineNum);

    virtual void resize(const Pos2d<int>& start, const Pos2d<int>& dim);
    virtual void load(const std::string& file, int line=0);
    int length() const { return (int)lines.size(); }
    Line& at(int idx) { return lines[idx]; }
    const Line& at(int idx) const { return lines[idx]; }

    virtual void drawBuffer(Editor& ed);
    void drawCursor(Editor& ed, const std::string& bg);
    virtual void drawStatusBar(Editor& ed);
    int drawLine(int y, const std::string& line, Editor& ed, int lineNum,
                 const std::string& fg, const std::string& bg);

    Pos2d<int> buffer2screen(const Pos2d<int>& loc) const;
    Cursor& getCursor() { return cursor; }
    const Cursor& getCursor() const { return cursor; }
    void addLine() { lines.push_back(Line()); }
    virtual int totalLinesNeeded() const;
    char charAt(const Pos2d<int>& pos) const;
    void lineUp();
    void lineDown();
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
    std::vector<std::string> regionAsStr() const;
    virtual void clear();
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


typedef bool(*ChoicesFilter)(const std::string&, const std::string&);

bool strFind(const std::string& line, const std::string& str);


class Choices {
public:
    Choices(ChoicesFilter cf): filter(cf) {}
    virtual ~Choices() {}
    virtual const std::string& at(int idx) const = 0;
    virtual int size() const = 0;
    virtual bool updateChoices(const std::string& str) { return false; }
    virtual std::string getFinalStr(int idx, const std::string& str) const = 0;
    bool match(const std::string& line, const std::string& str) const;
    bool match(int idx, const std::string& str) const;

private:
    ChoicesFilter filter;
};


class StringChoices: public Choices {
public:
    StringChoices(const std::vector<std::string>& arr,
                  ChoicesFilter cf=strFind);
    const std::string& at(int idx) const { return options[idx]; }
    std::string getFinalStr(int idx, const std::string& str) const;
    int size() const { return (int)options.size(); }

protected:
    std::vector<std::string> options;
};


class CmdMsgBar: public MultiLine {
public:
    CmdMsgBar();
    void resize(const Pos2d<int>& start, const Pos2d<int>& dim) override;
    void insert(char c) override;
    void insert(const char* str) override;
    void drawBuffer(Editor& ed) override;
    void load(const std::string& file, int line=0) override {}
    void drawStatusBar(Editor& ed) override {}
    void save() override {}
    void clear() override;
    void setMinLoc(int loc) { minLoc = loc; }
    int getMinStartLoc() const override { return minLoc; }
    int totalLinesNeeded() const override;
    void setChoices(Choices* ch) { choices = ch; }
    void clearChoices();
    bool usingChoices() const { return choices != nullptr; }
    std::string getStr() const { return lines[0].get().substr(minLoc); }
    std::string getFinalChoice() const;
    int linesNeeded(const std::string& str, int wid) const;
    void down();
    void up();
    void updateChoices();

private:
    /** useful during prompts, so as to not cross into the message itself! */
    int minLoc;
    /** external choices vector (usually options) that need to be rendered */
    Choices* choices;
    /** currently selected option */
    int optLoc;
};

}; // end namespace teditor
