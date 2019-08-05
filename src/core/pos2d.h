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
 * used for tracking locations of multiple cursors.
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
  Positions(const Positions& other): std::vector<Pos2di>(other) {}
  /** @} */

  /**
   * @defgroup Insert Update positions based on insertion
   * @{
   * @brief Update all the positions based on the characters inserted
   * @param a the positions
   * @param chars the characters
   */
  void added(const Strings& chars);
  void added(const std::string& chars);
  /** @} */

  /** check whether the two position lists are the same or not */
  bool operator==(const Positions& a) const { return same(*this, a); }

  /**
   * @brief move all positions on the same line by 'delta' chars
   * @param i reference position
   * @param delta amount of movement
   */
  void moveAllOnSameLine(size_t i, int delta);

  /**
   * @brief move all positions by 'delta' lines
   * @param i reference position
   * @param delta amount of movement
   */
  void moveLinesForAll(size_t i, int delta);

private:
  void addedImpl(size_t i, const std::string& chars);
};


/** Represents start/end of regions in a buffer */
class Regions: public Positions {
public:
  /** mark region start/end locations as specified in the input */
  void enable(const Positions& p);

  /**
   * @defgroup InsideCheck Check if the given location is in the regions
   * @{
   */
  bool isInside(int y, int x, const Positions& cu) const;
  bool isInside(int y, int x, const Positions& cu, int i) const;
  /** @} */

  /** get all region start/end markers */
  const Positions& getLocs() const { return *this; }
};

} // end namespace teditor
