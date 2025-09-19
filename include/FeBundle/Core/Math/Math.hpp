#ifndef INCLUDE_FEBUNDLE_CORE_MATH_MATH_HPP_
#define INCLUDE_FEBUNDLE_CORE_MATH_MATH_HPP_

#include "FeBundle/Core/Defines.hpp"

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

inline Vec2 Normalized(const Vec2& v) {
  float32 len = v.Length();
  return len > 0 ? Vec2(v.x / len, v.y / len) : Vec2(0, 0);
}

inline Vec2 Slide(const Vec2& motion, const Vec2& normal) {
  float32 dot = DotProduct(motion, normal);
  return motion - normal * dot;
}

} // namespace febundle::math

#endif // INCLUDE_FEBUNDLE_CORE_MATH_MATH_HPP_
