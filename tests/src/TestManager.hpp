#ifndef FEBUNDLE_TEST_MANAGER_HPP_
#define FEBUNDLE_TEST_MANAGER_HPP_

#include <FeBundle/Core/Defines.hpp>
#include <functional>

namespace febundle::tests {

using TestFn = std::function<bool()>;

struct TestEntry {
  TestFn func; 
  string description;
};

class TestManager {
public:
  TestManager(std::size_t maxTestCapacity = 2000);
  void RegisterTest(TestFn func, string description);
  void RunTests();

private:
  std::vector<TestEntry> _tests;
  std::size_t _registerCount{0};
  const std::size_t _cMaxTestCapacity;
};

}

#endif // FEBUNDLE_TEST_MANAGER_HPP_
