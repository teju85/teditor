#pragma once

#include <vector>
#include "utils.h"


namespace teditor {

/** anything that's a collection of 2d locations */
typedef std::vector<Pos2d<int> > Positions;


class Buffer;

class Cursor {
public:
    Cursor();
    void home(Buffer* ml);
    void lineEnd(Buffer* ml);
    void left(Buffer* ml);
    void right(Buffer* ml);
    void down(Buffer* ml);
    void up(Buffer* ml);
    void reset(Buffer* ml);
    void end(Buffer* ml);
    void pageDown(Buffer* ml, float jump);
    void pageUp(Buffer* ml, float jump);
    void nextPara(Buffer* ml);
    void previousPara(Buffer* ml);
    void nextWord(Buffer* ml);
    void previousWord(Buffer* ml);
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
    /** checks if the given line has any cursor in it */
    bool hasCursor(int line) const;

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

    friend class Buffer;
};


/** Represents start/end of regions in a buffer */
class Regions {
public:
    Regions(): locs() {}

    /** mark region start/end locations as specified in the input */
    void enable(const Positions& p);

    /** remove all region marks */
    void clear() { locs.clear(); }

    /**
     * @defgroup InsideCheck Check if the given location is in the regions
     * @{
     */
    bool isInside(int y, int x, const Cursor& cu) const;
    bool isInside(int y, int x, const Cursor& cu, int i) const;
    /** @} */

    /** returns the idx'th element */
    const Pos2d<int>& at(int idx) const { return locs[idx]; }

    /** number of regions (aka number of cursors) */
    int count() const { return (int)locs.size(); }

    /** get all region start/end markers */
    const Positions& getLocs() const { return locs; }

private:
    Positions locs;
};

}; // end namespace teditor
