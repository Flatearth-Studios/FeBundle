#ifndef INCLUDE_FEBUNDLE_CORE_MATH_MATH_HPP_
#define INCLUDE_FEBUNDLE_CORE_MATH_MATH_HPP_

#include "FeBundle/Core/Defines.hpp"
#include <array>
#include <cmath>

namespace febundle::core::math {

struct Vec2 {
  float32 x, y;

  Vec2();
  Vec2(float32 x, float32 y);
  Vec2 operator+(const Vec2 &other) const;
  Vec2 operator-(const Vec2 &other) const;
  Vec2 operator*(float32 val) const;
  Vec2 operator/(float32 val) const;
  void Normalize();
  float32 Length() const;

  static Vec2 One();
  static Vec2 Zero();
};

inline float32 DotProduct(const Vec2 &v1, const Vec2 &v2) {
  return v1.x * v2.x + v1.y * v2.y;
}

inline Vec2 Normalized(const Vec2 &v) {
  float32 len = v.Length();
  return len > 0 ? Vec2(v.x / len, v.y / len) : Vec2(0, 0);
}

inline Vec2 Slide(const Vec2 &motion, const Vec2 &normal) {
  float32 dot = DotProduct(motion, normal);
  return motion - normal * dot;
}

inline float32 Abs(float32 val) { return val < 0.0f ? val * (-1) : val; }

inline float32 Cos(float32 val) { return std::cos(val); }

inline float32 Sin(float32 val) { return std::sin(val); }

using Mat3Array = std::array<std::array<float32, 3>, 3>;

consteval Mat3Array IdentityMatrix3() noexcept {
  return {{{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}};
}

static constexpr Mat3Array IDENTITY_MAT3 = IdentityMatrix3();

struct Mat3 {
  Mat3Array matrix;

  Mat3() noexcept : matrix(IDENTITY_MAT3) {}

  Mat3(const Mat3Array &values) noexcept : matrix(values) {}

  static Mat3 Identity() { return Mat3(); }

  static constexpr Mat3 Translation(const Vec2 &v) noexcept {
    Mat3Array arr = IDENTITY_MAT3;
    arr[0][2] = v.x;
    arr[1][2] = v.y;
    return Mat3(arr);
  }

  static Mat3 Rotation(float32 radians) noexcept {
    float32 cos = Cos(radians);
    float32 sin = Sin(radians);

    Mat3Array arr = IDENTITY_MAT3;
    arr[0][0] = cos;
    arr[0][1] = -sin;
    arr[1][0] = sin;
    arr[1][1] = cos;
    return Mat3(arr);
  }

  static constexpr Mat3 Scale(const Vec2 &v) noexcept {
    Mat3Array arr = IDENTITY_MAT3;
    arr[0][0] = v.x;
    arr[1][1] = v.y;
    return Mat3(arr);
  }

  constexpr Mat3 operator*(const Mat3 &rhs) const noexcept {
    Mat3Array result{};
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        result[i][j] = matrix[i][0] * rhs.matrix[0][j] +
                       matrix[i][1] * rhs.matrix[1][j] +
                       matrix[i][2] * rhs.matrix[2][j];
      }
    }
    return Mat3(result);
  }

  constexpr Vec2 operator*(const Vec2 &vec2) const noexcept {
    return Vec2{
        matrix[0][0] * vec2.x + matrix[0][1] * vec2.y + matrix[0][2],
        matrix[1][0] * vec2.x + matrix[1][1] * vec2.y + matrix[1][2],
    };
  }

  constexpr std::array<float32, 3>& operator[](std::size_t row) noexcept {
      return matrix[row];
  }

  constexpr const std::array<float32, 3>& operator[](std::size_t row) const noexcept {
      return matrix[row];
  }
};

} // namespace febundle::core::math

#endif // INCLUDE_FEBUNDLE_CORE_MATH_MATH_HPP_
