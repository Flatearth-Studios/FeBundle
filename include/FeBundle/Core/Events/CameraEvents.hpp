#ifndef INCLUDE_FEBUNDLE_CORE_EVENTS_CAMERA_EVENTS_HPP_
#define INCLUDE_FEBUNDLE_CORE_EVENTS_CAMERA_EVENTS_HPP_

#include "FeBundle/Core/Scene/Entity.hpp"
#include "FeBundle/Core/Scene/Scene.hpp"

namespace febundle::core::events {

struct CameraSwapEvent {
  scene::Scene *pScene;
  scene::Entity targetCameraEntity;
  std::optional<float32> oWidth, oHeight;
};

struct ViewportChangedEvent {
  scene::Scene *pScene;
  std::optional<float32> oNewWidth, oNewHeight;
};

}

#endif // INCLUDE_FEBUNDLE_CORE_EVENTS_CAMERA_EVENTS_HPP_
