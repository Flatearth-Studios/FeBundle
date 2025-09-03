#include "FeBundle/Core/Clock.hpp"
#include <ctime>

namespace febundle {

void Clock::Start() {
  // Mark explicitly 'this' since they are public variables
  this->startTime = getAbsoluteTime();
}

void Clock::Update() {
  if (startTime != 0.0f) {
    this->elapsed = getAbsoluteTime() - startTime;
  }
}

void Clock::Stop() {
  this->startTime = 0.0f;
}

float64 Clock::NowTime() {
  return getAbsoluteTime();
}

float64 Clock::getAbsoluteTime() {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return now.tv_sec + now.tv_nsec * 0.000000001;
}

}
