#include "MathTests.hpp"
#include "../Expect.hpp"

namespace febundle::tests {

bool TestDefaultConstructor_Success() {
  Vec2 defaultVec;
  ASSERT_EQ_INT(defaultVec.x, 0);
  ASSERT_EQ_INT(defaultVec.y, 0);
  return true;
}

bool TestParameterConstructor_Success() {
  Vec2 paramVec = Vec2(12.0f, 5.43f);
  ASSERT_EQ_FLOAT(12.0f, paramVec.x);
  ASSERT_EQ_FLOAT(5.43f, paramVec.y);
  return true;
}

bool TestOperatorPlus_Success() {
  Vec2 vec1(1.0f, 0.0f);
  Vec2 vec2(0.0f, 1.0f);

  Vec2 res = vec1 + vec2;
  Vec2 expected(1.0f, 1.0f);
  ASSERT_EQ_VEC2(expected, res);
  return true;
}

bool TestOperatorMinus_Success() {
  Vec2 vec1(3.0f, 2.0f);
  Vec2 vec2(1.0f, 4.0f);

  Vec2 res = vec1 - vec2;
  Vec2 expected(2.0f, -2.0f);
  ASSERT_EQ_VEC2(expected, res);
  return true;
}

bool TestOperatorMultiply_Success() {
  Vec2 vec(2.0f, -3.0f);
  Vec2 res = vec * 2.5f;
  Vec2 expected(5.0f, -7.5f);
  ASSERT_EQ_VEC2(expected, res);
  return true;
}

bool TestOperatorDivide_Success() {
  Vec2 vec(10.0f, -5.0f);
  Vec2 res = vec / 5.0f;
  Vec2 expected(2.0f, -1.0f);
  ASSERT_EQ_VEC2(expected, res);
  return true;
}

bool TestOperatorDivide_ByZero() {
  Vec2 vec(4.0f, 2.0f);
  Vec2 res = vec / 0.0f;
  Vec2 expected(0.0f, 0.0f);
  ASSERT_EQ_VEC2(expected, res);
  return true;
}

bool TestNormalize_Success() {
  Vec2 vec(3.0f, 4.0f); // length = 5
  vec.Normalize();
  // after normalization: (0.6, 0.8)
  ASSERT_EQ_FLOAT(0.6f, vec.x);
  ASSERT_EQ_FLOAT(0.8f, vec.y);
  return true;
}

bool TestNormalize_ZeroVector() {
  Vec2 vec(0.0f, 0.0f);
  vec.Normalize(); // should not crash, should remain (0,0)
  ASSERT_EQ_VEC2(Vec2(0.0f, 0.0f), vec);
  return true;
}

bool TestLength_Success() {
  Vec2 vec(6.0f, 8.0f); // length = 10
  float32 len = vec.Length();
  ASSERT_EQ_FLOAT(10.0f, len);
  return true;
}

bool TestOneVector_Success() {
  Vec2 one = Vec2::One();
  ASSERT_EQ_VEC2(Vec2(1.0f, 1.0f), one);
  return true;
}

bool TestZeroVector_Success() {
  Vec2 zero = Vec2::Zero();
  ASSERT_EQ_VEC2(Vec2(0.0f, 0.0f), zero);
  return true;
}

bool TestDotProduct_Success() {
  Vec2 v1(2.0f, 3.0f);
  Vec2 v2(-1.0f, 4.0f);
  float32 result = DotProduct(v1, v2);
  // (2 * -1) + (3 * 4) = -2 + 12 = 10
  ASSERT_EQ_FLOAT(10.0f, result);
  return true;
}

bool TestNormalized_Success() {
  Vec2 v(3.0f, 4.0f); // length = 5
  Vec2 n = Normalized(v);
  ASSERT_EQ_FLOAT(0.6f, n.x);
  ASSERT_EQ_FLOAT(0.8f, n.y);
  return true;
}

bool TestNormalized_ZeroVector() {
  Vec2 v(0.0f, 0.0f);
  Vec2 n = Normalized(v);
  ASSERT_EQ_VEC2(Vec2(0.0f, 0.0f), n);
  return true;
}

bool TestSlide_Success() {
  Vec2 motion(1.0f, 1.0f);
  Vec2 normal(0.0f, 1.0f); // vertical normal

  Vec2 res = Slide(motion, normal);
  // dot = (1*0 + 1*1) = 1 → motion - normal*1 = (1,1) - (0,1) = (1,0)
  ASSERT_EQ_VEC2(Vec2(1.0f, 0.0f), res);
  return true;
}

bool TestAbs_Positive() {
  ASSERT_EQ_FLOAT(5.0f, Abs(5.0f));
  return true;
}

bool TestAbs_Negative() {
  ASSERT_EQ_FLOAT(7.0f, Abs(-7.0f));
  return true;
}

bool TestAbs_Zero() {
  ASSERT_EQ_FLOAT(0.0f, Abs(0.0f));
  return true;
}

void MathRegisterTests(TestManager &tm) {
  TestMetadata test;
  test.name = "Math Tests";

  std::vector<std::string> descriptions = {
      "default construction of Vec2",
      "constructor of Vec2 with parameters",
      "operator + of Vec2",
      "operator - of Vec2",
      "operator * of Vec2",
      "operator / of Vec2",
      "operator / with divisor 0",
      "normalize non-zero Vec2",
      "normalize zero Vec2",
      "length of Vec2",
      "Vec2::One static method",
      "Vec2::Zero static method",
      "DotProduct of two Vec2",
      "Normalized of non-zero Vec2",
      "Normalized of zero Vec2",
      "Slide vector along normal",
      "Abs of positive value",
      "Abs of negative value",
      "Abs of zero value"};

  std::vector<TestEntry> tests = {
      {TestDefaultConstructor_Success, descriptions[0]},
      {TestParameterConstructor_Success, descriptions[1]},
      {TestOperatorPlus_Success, descriptions[2]},
      {TestOperatorMinus_Success, descriptions[3]},
      {TestOperatorMultiply_Success, descriptions[4]},
      {TestOperatorDivide_Success, descriptions[5]},
      {TestOperatorDivide_ByZero, descriptions[6]},
      {TestNormalize_Success, descriptions[7]},
      {TestNormalize_ZeroVector, descriptions[8]},
      {TestLength_Success, descriptions[9]},
      {TestOneVector_Success, descriptions[10]},
      {TestZeroVector_Success, descriptions[11]},
      {TestDotProduct_Success, descriptions[12]},
      {TestNormalized_Success, descriptions[13]},
      {TestNormalized_ZeroVector, descriptions[14]},
      {TestSlide_Success, descriptions[15]},
      {TestAbs_Positive, descriptions[16]},
      {TestAbs_Negative, descriptions[17]},
      {TestAbs_Zero, descriptions[18]},
  };

  auto regFunc = [tests]() -> TestMetadata {
    return TestMetadata{
        .name = "Math",
        .callbacks = std::move(tests),
    };
  };

  tm.RegisterTests(regFunc);
}
} // namespace febundle::tests
