#define FE_DEBUG
#include "TestManager.hpp"
#include "FeBundle/Core/Clock.hpp"
#include "FeBundle/Core/Logger.hpp"

namespace febundle::tests {

TestManager::TestManager(std::size_t maxTestCapacity)
    : _cMaxTestCapacity(maxTestCapacity) {
  _tests.reserve(maxTestCapacity);
}

void TestManager::RegisterTest(TestFn func, string description) {
  if (_tests.size() == _cMaxTestCapacity) {
    FLOG_WARN(
        "attempt to register test when maximum capacity is already reached");
    return;
  }
  _tests.emplace_back(TestEntry{.func = func, .description = description});
}

void TestManager::RunTests() {
  uint32 passed = 0;
  uint32 failed = 0;
  uint32 count = _tests.size();

  Clock totalTimeClock;
  totalTimeClock.Start();

  for (uint32 i = 0; i < count; i++) {
    Clock testTimeClock;
    testTimeClock.Start();
    bool result = _tests[i].func();
    testTimeClock.Update();

    if (result == true) {
      passed++;
    } else {
      LOG_ERROR("[FE/TESTS] - [FAILED]: {}", _tests[i].description);
      failed++;
    }

    string status;
    if (result == true) {
      status = std::format("[FE/TESTS] - [✓] PASS: {}", passed);
    } else {
      status = std::format("[FE/TESTS] - [✗] FAIL: {}", failed);
    }

    testTimeClock.Update();
    totalTimeClock.Update();
    LOG_INFO("{}. Executed {} of {} in ({} sec / {} sec total)", status,
             i + 1, count, testTimeClock.elapsed, totalTimeClock.elapsed);
  }

  totalTimeClock.Stop();
  uint32 total = passed + failed;
  LOG_INFO("[FE/TESTS] - Results: {} passed | {} failed | {} total", passed,
           failed, total);
}

} // namespace febundle::tests
