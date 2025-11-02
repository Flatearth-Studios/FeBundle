#ifndef INCLUDE_FEBUNDLE_CORE_SYSTEMS_CAMERA_SYSTEM_HPP_
#define INCLUDE_FEBUNDLE_CORE_SYSTEMS_CAMERA_SYSTEM_HPP_

#include "FeBundle/Core/Events/EventBus.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include "FeBundle/Core/Scene/Entity.hpp"
#include "FeBundle/Core/Scene/Scene.hpp"
namespace febundle::systems {

class CameraSystem {
public:
  explicit CameraSystem(core::events::EventBus &evtBus);

  void Update(float32 deltaTime);
  const scene::CameraPtr &ActiveCamera() const noexcept;

private:
  void update(scene::Scene *pScene, float32 viewportWidth,
              float32 viewportHeight,
              std::optional<scene::Entity> entity = std::nullopt,
              bool reload = true);

  bool shouldReuseCamera(bool reload) const noexcept;
  bool assignCamera(scene::Scene *pScene);

  void updateActiveCameraInternals(scene::Scene *pScene, float32 width,
                                   float32 height,
                                   std::optional<scene::Entity> entity);
  scene::Transform *
  getCameraTransform(scene::Scene *pScene,
                     std::optional<scene::Entity> entity = std::nullopt);
  void subscribeToEvents();

private:
  core::events::EventBus &_eventBus;
  scene::CameraPtr _pActiveCamera;
  scene::Transform *_pCameraTransform;
};

} // namespace febundle::systems

#endif // INCLUDE_FEBUNDLE_CORE_SYSTEMS_CAMERA_SYSTEM_HPP_
