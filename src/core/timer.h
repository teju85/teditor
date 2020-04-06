#pragma once

#include <chrono>
#include <stdint.h>
#include "utils.h"

namespace teditor {

/**
 * @brief Main object used for timing event
 */
class Timer {
 public:
  Timer() : running(false) {}

  void start() {
    ASSERT(!running, "Timer is already running!");
    _start = std::chrono::steady_clock::now();
    running = true;
  }

  bool isRunning() const { return running; }

  /**
   * @brief Stop the timer and measure the elapsed time
   * @return the elapsed time in microseconds
   */
  uint64_t stop() {
    ASSERT(running, "Timer is not running!");
    using namespace std::chrono;
    auto _stop = steady_clock::now();
    running = false;
    auto diff = duration_cast<microseconds>(_stop - _start).count();
    return (uint64_t)diff;
  }

 private:
  bool running;
  std::chrono::steady_clock::time_point _start;
};  // class Timer

}  // namespace teditor
