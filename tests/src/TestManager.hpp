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

struct TestMetadata {
  string name;
  std::vector<TestEntry> callbacks;
};

using TestRegFn = std::function<TestMetadata()>;

class TestManager {
public:
  TestManager(std::size_t maxTestCapacity = 2000);
  void RegisterTests(TestRegFn regFunc);
  void RunTests();

private:
  void prepareFor(const string &testName);

private:
  umap<string, std::vector<TestEntry>> _tests;
  std::size_t _registerCount{0};
  const std::size_t _cMaxTestCapacity;
};

}

#endif // FEBUNDLE_TEST_MANAGER_HPP_
