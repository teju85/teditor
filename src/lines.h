#pragma once

#include <stack>
#include <string>
#include <vector>
#include "line.h"
#include "pos2d.h"


namespace teditor {

/**
 * @brief Collection of Line objects, thereby representing the workhorse behind
 * the Buffer class. However, this has no concept of Regions/Cursors! It only
 * knows how to insert/delete contents in it, and also how to perform undo/redo
 * on those operations.
 */
class Lines: public std::vector<Line> {
public:
    /** ctor */
    Lines();

    /**
     * @defgroup Stack Working with history of operations for undo/redo
     * @{
     */
    /** undo the operations. Returns false if there's nothing to undo */
    bool undo();
    /** redo the operations. Returns false if there's nothing to redo */
    bool redo();
    /** start capturing of operations */
    void startCapture() { capture = true; }
    /** stop capturing of operations */
    void stopCapture() { capture = false; }
    /** whether current mode is set to capture operations or not */
    bool canCapture() const { return capture; }
    /** @} */

    /**
     * @defgroup Operations operations that can be performed on Lines
     * @{
     */
    /** insert a character at a given location */
    void insert(const Pos2di& pos, char c);
    /** insert a set of characters from a given location */
    void insert(const Pos2di& pos, const std::string& c);
    /** remove a character at the given location */
    void remove(const Pos2di& pos);
    /** remove a set of characters inside the given range */
    void remove(const Pos2di& start, const Pos2di& end);
    /** @} */

private:
    /** the operation type */
    enum OpType {
        /** insertion operation */
        OpInsert = 0,
        /** backspace operation */
        OpDelete
    };


    /**
     * @brief The state before/after applying insertion/deletion operations on the
     * Lines object (aka Buffer)
     */
    class OpData {
    public:
        /**
         * @defgroup Ctors Different constructors for each operation
         * @{
         */
        /** insertion operation */
        OpData(const Pos2di& b, const std::string& c): before(b), after(-1, -1),
                                                       chars(c), type(OpInsert) {}
        /** delete operation */
        OpData(const Pos2di& b, const Pos2di& a, const std::string& c):
            before(b), after(a), chars(c), type(OpDelete) {}
        /** @} */

    private:
        /** from where the operation started */
        Pos2di before;
        /** till where the operation was performed */
        Pos2di after;
        /** characters that were inserted/deleted in the above range */
        std::string chars;
        /** type of operation */
        OpType type;

        friend class Lines;
    }; // end class OpData


    typedef std::stack<OpData> OpStack;

    void applyOp(const OpData& op);
    void applyInsertOp(const OpData& op);
    void applyDeleteOp(const OpData& op);

    void clearStack(OpStack& st);

    /** whether to capture the operations for undo/redo */
    bool capture;
    /** stack of operations for undo */
    OpStack undoStack;
    /** stack of operations for redo */
    OpStack redoStack;
};

} // end namespace teditor
