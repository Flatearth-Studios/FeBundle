#ifndef INCLUDE_FEBUNDLE_CORE_CLOCK_HPP_
#define INCLUDE_FEBUNDLE_CORE_CLOCK_HPP_

#include "Defines.hpp"

namespace febundle {

class Clock {
public:
  void Start();
  void Update();
  void Stop();
  float64 NowTime();

public:
  float64 startTime, elapsed;

private:
  float64 getAbsoluteTime();
};

}

#endif // INCLUDE_FEBUNDLE_CORE_CLOCK_HPP_
