#ifndef FEBUNDLE_EXPECT_HPP_
#define FEBUNDLE_EXPECT_HPP_

#include <FeBundle/Core/Logger.hpp>
#include <FeBundle/Core/Math/Math.hpp> // for Abs()

namespace febundle::tests {

#define ASSERT_EQ_INT(expected, actual)                                        \
  do {                                                                         \
    auto _expected = (expected);                                               \
    auto _actual = (actual);                                                   \
    if (_actual != _expected) {                                                \
      LOG_ERROR("--> Expected {}, but got: {}. File: {}:{}",                   \
                static_cast<long long>(_expected),                             \
                static_cast<long long>(_actual), __FILE__, __LINE__);          \
      return false;                                                            \
    }                                                                          \
  } while (0)

#define ASSERT_EQ_FLOAT(expected, actual)                                      \
  do {                                                                         \
    float _expected = (expected);                                              \
    float _actual = (actual);                                                  \
    if (core::math::Abs(_actual - _expected) > 0.001f) {                       \
      LOG_ERROR("--> Expected {}, but got: {}. File: {}:{}",                   \
                _expected, _actual, __FILE__, __LINE__);                       \
      return false;                                                            \
    }                                                                          \
  } while (0)

#define ASSERT_EQ_VEC2(expected, actual)                                       \
  do {                                                                         \
    core::math::Vec2 _expected = (expected);                                   \
    core::math::Vec2 _actual = (actual);                                       \
    if (_actual.x != _expected.x || _actual.y != _expected.y) {                \
      LOG_ERROR("--> Expected Vec2({}, {}), but got Vec2({}, {}). File: {}:{}",\
                _expected.x, _expected.y, _actual.x, _actual.y, __FILE__,      \
                __LINE__);                                                     \
      return false;                                                            \
    }                                                                          \
  } while (0)

// ---------- NEW ASSERTS ----------

#define ASSERT_GT_FLOAT(actual, threshold)                                     \
  do {                                                                         \
    float _actual = (actual);                                                  \
    float _threshold = (threshold);                                            \
    if (!(_actual > _threshold)) {                                             \
      LOG_ERROR("--> Expected > {}, but got: {}. File: {}:{}",                 \
                _threshold, _actual, __FILE__, __LINE__);                      \
      return false;                                                            \
    }                                                                          \
  } while (0)

#define ASSERT_LT_FLOAT(actual, threshold)                                     \
  do {                                                                         \
    float _actual = (actual);                                                  \
    float _threshold = (threshold);                                            \
    if (!(_actual < _threshold)) {                                             \
      LOG_ERROR("--> Expected < {}, but got: {}. File: {}:{}",                 \
                _threshold, _actual, __FILE__, __LINE__);                      \
      return false;                                                            \
    }                                                                          \
  } while (0)

#define ASSERT_GE_FLOAT(actual, threshold)                                     \
  do {                                                                         \
    float _actual = (actual);                                                  \
    float _threshold = (threshold);                                            \
    if (!(_actual >= _threshold)) {                                            \
      LOG_ERROR("--> Expected >= {}, but got: {}. File: {}:{}",                \
                _threshold, _actual, __FILE__, __LINE__);                      \
      return false;                                                            \
    }                                                                          \
  } while (0)

#define ASSERT_LE_FLOAT(actual, threshold)                                     \
  do {                                                                         \
    float _actual = (actual);                                                  \
    float _threshold = (threshold);                                            \
    if (!(_actual <= _threshold)) {                                            \
      LOG_ERROR("--> Expected <= {}, but got: {}. File: {}:{}",                \
                _threshold, _actual, __FILE__, __LINE__);                      \
      return false;                                                            \
    }                                                                          \
  } while (0)

} // namespace febundle::tests

#endif // FEBUNDLE_EXPECT_HPP_
