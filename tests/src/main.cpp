#define FE_DEBUG
#include "Math/MathTests.hpp"
#include <FeBundle/Core/Logger.hpp>
#include "TestManager.hpp"

using namespace febundle::tests;

int main() {
  TestManager tm(1);
  
  MathRegisterTests(tm);
  LOG_DEBUG("Starting tests...");
  tm.RunTests();
  return 0;
}
