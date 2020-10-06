#pragma once

#include <stdint.h>
#include <unordered_map>
#include <chrono>
#include <string>

namespace teditor {

/**
 * @brief Main object used for timing event
 */
class Timer {
 public:
  Timer() : running(false) {}
  void start();
  bool isRunning() const { return running; }
  void stop();
  double elapsed();

 private:
  bool running;
  std::chrono::steady_clock::time_point start_;
  std::chrono::steady_clock::time_point stop_;
};  // class Timer

typedef std::unordered_map<std::string, Timer> Timers;

/** get the timer object of the given name */
Timer& getTimer(const std::string& name);

/** start a named timer */
void tic(const std::string& name);

/** stop the previously started named timer */
void toc(const std::string& name);

/** remove all the named timers created so far */
void resetTimers();

}  // namespace teditor
