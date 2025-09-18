#define FE_DEBUG
#include "FeBundle/Core/Math/Math.hpp"
#include "FeBundle/Core/Logger.hpp"
#include <cmath>

namespace febundle::math {

Vec2::Vec2() : x(0), y(0) {}

Vec2::Vec2(float32 x, float32 y) : x(x), y(y) {}

Vec2 Vec2::operator+(const Vec2 &other) const {
  return Vec2(x + other.x, y + other.y);
}

Vec2 Vec2::operator-(const Vec2 &other) const {
  return Vec2(x - other.x, y - other.y);
}

Vec2 Vec2::operator*(float32 val) const { return Vec2(x * val, y * val); }

Vec2 Vec2::operator/(float32 val) const {
  return val != 0 ? Vec2(x / val, y / val) : Vec2(0, 0);
}

void Vec2::Normalize() {
  const float32 len = Length();
  if (len == 0) {
    FLOG_WARN("normalization resulted in division by zero");
    return;
  }

  x /= len;
  y /= len;
}

float32 Vec2::Length() const { return std::sqrt(DotProduct(*this, *this)); }

Vec2 Vec2::One() { return Vec2(1.0f, 1.0f); }

Vec2 Vec2::Zero() { return Vec2(0.0f, 0.0f); }

} // namespace febundle::math
