#include "Scene/SceneTests.hpp"
#define FE_DEBUG
#include "Math/MathTests.hpp"
#include "Clock/ClockTests.hpp"
#include "Events/EventQueueTests.hpp"
#include <FeBundle/Core/Logger.hpp>
#include "TestManager.hpp"

using namespace febundle::tests;

int main() {
  TestManager tm;
  MathRegisterTests(tm);
  ClockRegisterTests(tm);
  EventQueueRegisterTests(tm);
  StoreRegisterTests(tm);
  SceneRegisterTests(tm);

  LOG_DEBUG("Starting tests...");
  tm.RunTests();
  return 0;
}
