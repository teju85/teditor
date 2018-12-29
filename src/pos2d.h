#pragma once

#include "utils.h"


namespace teditor {

/**
 * @brief Any 2D position
 * @tparam T Type of position
 */
template <typename T>
struct Pos2d {
    T x, y;

    Pos2d(T x_=0, T y_=0): x(x_), y(y_) {}

    bool operator>(const Pos2d<T>& b) const {
        return (y > b.y) || (y == b.y && x > b.x);
    }

    bool operator>=(const Pos2d<T>& b) const {
        return (y > b.y) || (y == b.y && x >= b.x);
    }

    bool operator==(const Pos2d<T>& b) const {
        return (x == b.x) && (y == b.y);
    }

    bool operator!=(const Pos2d<T>& b) const {
        return (x != b.x) || (y != b.y);
    }

    bool operator<=(const Pos2d<T>& b) const {
        return (y < b.y) || (y == b.y && x <= b.x);
    }

    bool operator<(const Pos2d<T>& b) const {
        return (y < b.y) || (y == b.y && x < b.x);
    }

    /**
     * @brief Finds the start/end points for the range defined by this and
     * the other position.
     * @param start the start location
     * @param end the end location
     * @param other the other position
     * @return -1 if the vertical location is earlier, 0 if equal and +1 if
     * earlier than the other
     */
    int find(Pos2d<T>& start, Pos2d<T>& end, const Pos2d<T>& other) const {
        if(y < other.y) {
            start = *this;
            end = other;
            return -1;
        } else if(y > other.y) {
            start = other;
            end = *this;
            return 1;
        } else {
            start = {std::min(x, other.x), y};
            end = {std::max(x, other.x), y};
            return 0;
        }
    }

    /** move the pointer ahead based on the characters in the input */
    Pos2d& operator+=(const std::string& chars) {
        for(const auto& c : chars) {
            if(c == '\n') {
                ++y;
                x = 0;
            } else {
                ++x;
            }
        }
        return *this;
    }
};


/** integer pos2d */
typedef Pos2d<int> Pos2di;


/**
 * @brief anything that's a collection of 2d locations. This is specifically
 * used for tracking locations of multiple cursors. The first location is always
 * the absolute position of the cursor and all others are always the delta from
 * *below* it. Meaning, in case you want to add a new cursor above the first
 * location, it'll then be made the first cursor and all others following it
 * will be relative to this new one!
 */
class Positions: public std::vector<Pos2di> {
public:
    /**
     * @defgroup Constructors
     * @{
     */
    Positions(): std::vector<Pos2di>() {}
    Positions(size_t s): std::vector<Pos2di>(s) {}
    Positions(const std::initializer_list<Pos2di>& pos):
        std::vector<Pos2di>(pos) {}
    /** @} */

    /**
     * @brief Update all the positions based on the characters
     * @param a the positions
     * @param chars the characters
     */
    void update(const Strings& chars);

    /** check whether the two position lists are the same or not */
    bool operator==(const Positions& a) const;
};

} // end namespace teditor
