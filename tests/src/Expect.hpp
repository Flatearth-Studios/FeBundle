#ifndef FEBUNDLE_EXPECT_HPP_
#define FEBUNDLE_EXPECT_HPP_

#include <FeBundle/Core/Logger.hpp>

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

} // namespace febundle::tests

#endif // FEBUNDLE_EXPECT_HPP_
