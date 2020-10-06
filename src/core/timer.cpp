#include "timer.h"
#include "utils.h"

namespace teditor {

void Timer::start() {
  ASSERT(!running, "start: Timer is already running!");
  start_ = std::chrono::steady_clock::now();
  running = true;
}

void Timer::stop() {
  ASSERT(running, "stop: Timer is not running!");
  stop_ = std::chrono::steady_clock::now();
  running = false;
}

double Timer::elapsed() {
  if (running) stop();
  return std::chrono::duration<double>(stop_ - start_).count();
}

Timers& getTimers() {
  static Timers t;
  return t;
}

Timer& getTimer(const std::string& name) {
  auto& t = getTimers();
  auto itr = t.find(name);
  ASSERT(itr != t.end(), "Unable to find timer: '%s'!", name.c_str());
  return itr->second;
}

void tic(const std::string& name) {
  auto& t = getTimers();
  auto itr = t.find(name);
  if (itr == t.end()) {
    t[name] = Timer();
    itr = t.find(name);
  }
  itr->second.start();
}

void toc(const std::string& name) {
  auto& t = getTimer(name);
  t.stop();
}

void resetTimers() { getTimers().clear(); }

}  // namespace teditor
