#include "ClockTests.hpp"
#include "../Expect.hpp"
#include "FeBundle/Core/Clock.hpp"
#include <thread>   // for sleep_for
#include <chrono>   // for milliseconds

namespace febundle::tests {

bool TestClock_StartAndUpdate() {
  Clock c;
  c.Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  c.Update();

  ASSERT_GT_FLOAT(c.elapsed, 0.0); // elapsed should be positive
  return true;
}

bool TestClock_ElapsedIncreases() {
  Clock c;
  c.Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  c.Update();
  double first = c.elapsed;

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  c.Update();
  double second = c.elapsed;

  ASSERT_GT_FLOAT(second, first); // elapsed should grow
  return true;
}

bool TestClock_StopResetsStartTime() {
  Clock c;
  c.Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(2));
  c.Stop();
  ASSERT_EQ_FLOAT(0.0, c.startTime);
  return true;
}

bool TestClock_NowTimePositive() {
  Clock c;
  double now = c.NowTime();
  ASSERT_GT_FLOAT(now, 0.0); // should always be > 0
  return true;
}

void ClockRegisterTests(TestManager &tm) {
  std::vector<TestEntry> tests = {
      {TestClock_StartAndUpdate,   "Clock start and update basic"},
      {TestClock_ElapsedIncreases, "Clock elapsed increases with time"},
      {TestClock_StopResetsStartTime, "Clock stop resets startTime"},
      {TestClock_NowTimePositive,  "Clock NowTime returns positive value"},
  };

  auto regFunc = [tests]() -> TestMetadata {
    return TestMetadata{
      .name = "Clock",
      .callbacks = std::move(tests),
    }; 
  };

  tm.RegisterTests(regFunc);
}

}
