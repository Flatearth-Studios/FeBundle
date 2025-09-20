#define FE_DEBUG
#include "TestManager.hpp"
#include "FeBundle/Core/Clock.hpp"
#include "FeBundle/Core/Logger.hpp"

namespace febundle::tests {

TestManager::TestManager(std::size_t maxTestCapacity)
    : _cMaxTestCapacity(maxTestCapacity) {}

void TestManager::RegisterTests(TestRegFn regFunc) {
  const TestMetadata meta = regFunc();
  auto it = _tests.find(meta.name);
  if (it == _tests.end()) {
    prepareFor(meta.name);
  }

  for (const auto &callback : meta.callbacks) {
    if (_tests[meta.name].size() >= _cMaxTestCapacity) {
      FLOG_WARN("attempt to add tests but reached max capacity for test '{}'",
                meta.name);
      return;
    }
    _tests[meta.name].emplace_back(TestEntry{
        .func = callback.func,
        .description = callback.description,
    });
  }
}

void TestManager::RunTests() {
  Clock totalTimeClock;
  totalTimeClock.Start();

  uint32 passed = 0;
  uint32 failed = 0;
  for (auto [testName, tests] : _tests) {
    std::size_t count = tests.size();
    Clock testTimeClock;
    testTimeClock.Start();
    int32 i = 0;
    LOG_DEBUG("Starting tests for: {}", testName);
    for (auto testFn : tests) {
      bool result = testFn.func();
      testTimeClock.Update();

      if (result == true) {
        passed++;
      } else {
        LOG_ERROR("[FE/TESTS] - [FAILED]: {}", testFn.description);
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
      LOG_INFO("{}. Executed {} of {} in ({:.4f} sec / {:.4f} sec total)",
               status, i + 1, count, testTimeClock.elapsed,
               totalTimeClock.elapsed);
      i++;
    }
  }

  totalTimeClock.Stop();
  uint32 total = passed + failed;
  LOG_INFO("[FE/TESTS] - Results: {} passed | {} failed | {} total", passed,
           failed, total);
}

void TestManager::prepareFor(const string &testName) {
  auto it = _tests.find(testName);
  if (it != _tests.end()) {
    return;
  }

  std::vector<TestEntry> in;
  in.reserve(_cMaxTestCapacity);
  _tests[testName] = std::move(in);
}

} // namespace febundle::tests
