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

  const Pos2d<T>& operator=(const Pos2d<T>& in) {
    x = in.x;
    y = in.y;
    return *this;
  }

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
  T find(Pos2d<T>& start, Pos2d<T>& end, const Pos2d<T>& other) const {
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

  /**
   * @brief Check if the given location is in the regions
   */
  bool isInside(T _y, T _x, const Pos2d<T>& cu) const {
    if (y == T(-1) && x == T(-1)) return false;
    Pos2d<T> start, end;
    find(start, end, cu);
    if(start.y < _y && _y < end.y) return true;
    else if(start.y == end.y && start.y == _y) {
      if(start.x <= _x && _x <= end.x) return true;
    } else if(start.y == _y && _x >= start.x) return true;
    else if(end.y == _y && _x <= end.x) return true;
    return false;
  }
};


/** unsigned integer pos2d */
typedef Pos2d<unsigned> Pos2du;
/** cursor */
typedef Pos2d<size_t> Point;

} // end namespace teditor
