#ifndef INCLUDE_FEBUNDLE_CORE_CAMERA_CAMERA_HPP_
#define INCLUDE_FEBUNDLE_CORE_CAMERA_CAMERA_HPP_

#include "FeBundle/Core/Defines.hpp"
#include "FeBundle/Core/Math/Math.hpp"

namespace febundle::camera {

struct Camera {
  float32 zoom{1.0f};
  bool active{true};

  core::math::Mat3 view, projection;
  core::math::Vec2 viewportSize;

  void UpdateView(const core::math::Vec2 &position,
                  float32 rotRadians) noexcept;
  void UpdateProjection(float32 width, float32 height) noexcept;

  static Camera Default(float32 width, float32 height) noexcept;
};

} // namespace febundle::camera

#endif // INCLUDE_FEBUNDLE_CORE_CAMERA_CAMERA_HPP_
