#define FE_DEBUG
#include "FeBundle/Core/Camera/Camera.hpp"
#include "FeBundle/Core/Math/Math.hpp"

namespace febundle::camera {

using namespace core::math;

void Camera::UpdateView(const Vec2 &position, float32 rotRadians) noexcept {
  const float32 cos = Cos(-rotRadians); 
  const float32 sin = Sin(-rotRadians);

  Mat3Array arr = IDENTITY_MAT3;
  arr[0][0] = cos * zoom;
  arr[0][1] = -sin * zoom;
  arr[0][2] = -position.x * cos * zoom + position.y * sin * zoom;
  arr[1][0] = sin * zoom;
  arr[1][1] = cos * zoom;
  arr[1][2] = -position.x * sin * zoom - position.y * cos * zoom;

  view = Mat3(arr);
}

void Camera::UpdateProjection(float32 width, float32 height) noexcept {
    projection = Mat3::Identity();
    viewportSize = Vec2(width, height);
}

Camera Camera::Default(float32 width, float32 height) noexcept {
  Camera cam;
  cam.UpdateProjection(width, height);
  cam.UpdateView(Vec2{0.0f, 0.0f}, 0.0f);
  return cam;
}

}
