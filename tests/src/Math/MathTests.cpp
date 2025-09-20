#include "MathTests.hpp"
#include "../Expect.hpp"

namespace febundle::tests {

bool TestDefaultConstructor_Success() {
  Vec2 defaultVec;
  ASSERT_EQ_INT(defaultVec.x, 0);
  ASSERT_EQ_INT(defaultVec.y, 0);
  return true;
}

void MathRegisterTests(TestManager &tm) {
  tm.RegisterTest(TestDefaultConstructor_Success,
                  "default construction of Vec2");
}

} // namespace febundle::tests
