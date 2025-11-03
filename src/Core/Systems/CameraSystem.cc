#define FE_DEBUG
#include "FeBundle/Core/Systems/CameraSystem.hpp"
#include "FeBundle/Core/Events/CameraEvents.hpp"
#include "FeBundle/Core/Events/EventQueue.hpp"
#include "FeBundle/Core/Events/SceneLoadedEvent.hpp"
#include "FeBundle/Core/Logger.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include "FeBundle/Core/Scene/Entity.hpp"
#include "FeBundle/Core/Scene/Scene.hpp"
#include <cassert>
#include <optional>

namespace febundle::systems {

using namespace scene;

CameraSystem::CameraSystem(core::events::EventBus &evtBus) : _eventBus(evtBus) {
  subscribeToEvents();
}

void CameraSystem::Update(float32 deltaTime) {
  using namespace core::math;
  if (_pCameraTransform == nullptr || _pActiveCamera == nullptr) {
    return;
  }

  if (_pCameraTransform->dirty) {
    FLOG_TRACE("camera changed");
    _pActiveCamera->UpdateView(Vec2(_pCameraTransform->x, _pCameraTransform->y),
                               _pCameraTransform->rot);
    _pCameraTransform->dirty = false;
  }
}

const scene::CameraPtr &CameraSystem::ActiveCamera() const noexcept {
  return _pActiveCamera;
}

void CameraSystem::update(scene::Scene *pScene, float32 viewportWidth,
                          float32 viewportHeight,
                          std::optional<scene::Entity> entity, bool reload) {
  using namespace core::math;
  if (pScene == nullptr) {
    FLOG_ERROR("cannot reload camera with a nullptr scene");
    return;
  }

  if (shouldReuseCamera(reload)) {
    return updateActiveCameraInternals(pScene, viewportWidth, viewportHeight,
                                       entity);
  }

  if (!assignCamera(pScene)) {
    FLOG_WARN("no active camera found in scene");
    return;
  }

  return updateActiveCameraInternals(pScene, viewportWidth, viewportHeight,
                                     entity);
}

bool CameraSystem::shouldReuseCamera(bool reload) const noexcept {
  return _pActiveCamera != nullptr && !reload;
}

bool CameraSystem::assignCamera(scene::Scene *pScene) {
  if (pScene == nullptr) {
    return false;
  }

  for (auto &[entity, comps] : pScene->AccessAll()) {
    if (!comps.contains(scene::Component::Camera)) {
      continue;
    }

    auto *camComp = pScene->GetComponent<scene::Camera>(entity);
    if (camComp && camComp->pCamera && camComp->pCamera->active) {
      _pActiveCamera = camComp->pCamera;
      return true;
    }
  }

  return false;
}

void CameraSystem::updateActiveCameraInternals(
    scene::Scene *pScene, float32 width, float32 height,
    std::optional<scene::Entity> entity) {
  if (!_pActiveCamera) {
    FLOG_ERROR("no active camera to update");
    return;
  }

  _pCameraTransform = getCameraTransform(pScene, entity);

  if (_pCameraTransform == nullptr) {
    FLOG_WARN("camera has no transform; using origin");
    _pActiveCamera->UpdateProjection(width, height);
    _pActiveCamera->UpdateView({0, 0}, 0.0f);
    return;
  }

  _pActiveCamera->UpdateProjection(width, height);
  _pActiveCamera->UpdateView({_pCameraTransform->x, _pCameraTransform->y},
                             _pCameraTransform->rot);
}

scene::Transform *
CameraSystem::getCameraTransform(scene::Scene *pScene,
                                 std::optional<scene::Entity> entity) {
  if (pScene == nullptr) {
    return nullptr;
  }

  if (entity.has_value()) {
    if (auto *t = pScene->GetComponent<scene::Transform>(entity.value())) {
      return t;
    }
  }

  // fallback: find transform of first camera in scene
  for (auto &[ent, comps] : pScene->AccessAll()) {
    if (comps.contains(scene::Component::Camera)) {
      return pScene->GetComponent<scene::Transform>(ent);
    }
  }
  return nullptr;
}

void CameraSystem::subscribeToEvents() {
  core::events::EventSubscription<core::events::SceneLoadedEvent> sceneLoadSub{
      .subscriber = "CameraSystem",
      .callback = [&](const core::events::SceneLoadedEvent &evt)
          -> std::expected<void, Error> {
        if (evt.pScene == nullptr) {
          FLOG_ERROR("attempt to load nullptr scene");
          return std::unexpected{Error(ErrorName::LoadSceneEvent)};
        }

        if (evt.pScene->Type() != scene::SceneType::World) {
          return {};
        }

        float32 vWidth = evt.oViewportWidth.value_or(1280.0f);
        float32 vHeight = evt.oViewportHeight.value_or(960.0f);

        if (evt.oViewportHeight == std::nullopt ||
            evt.oViewportWidth == std::nullopt) {
          FLOG_WARN("malformed camera event, using fallback viewport {}x{}",
                    vWidth, vHeight);
        }

        this->update(evt.pScene, vWidth, vHeight);
        return {};
      }};

  if (auto res = _eventBus.Subscribe(sceneLoadSub); !res.has_value()) {
    FLOG_ERROR("failed to subscribe to SceneLoadedEvent");
  }

  core::events::EventSubscription<core::events::CameraSwapEvent> camSwapSub{
      .subscriber = "CameraSystem",
      .callback = [&](const core::events::CameraSwapEvent &evt)
          -> std::expected<void, Error> {
        if (evt.pScene == nullptr) {
          FLOG_ERROR("attempt to load nullptr scene");
          return std::unexpected{Error(ErrorName::CameraSwapEvent)};
        }

        auto *cameraComponent =
            evt.pScene->GetComponent<scene::Camera>(evt.targetCameraEntity);
        if (cameraComponent == nullptr) {
          FLOG_WARN("entity has no valid camera component");
          return {};
        }

        if (cameraComponent->pCamera == nullptr) {
          FLOG_WARN("no camera attached to this component");
          return {};
        }

        if (evt.oHeight != std::nullopt && evt.oWidth != std::nullopt) {
          update(evt.pScene, evt.oWidth.value(), evt.oHeight.value());
        }

        _pActiveCamera = cameraComponent->pCamera;
        return {};
      },
  };

  if (auto res = _eventBus.Subscribe(camSwapSub); !res.has_value()) {
    FLOG_ERROR("failed to subscribe to CameraSwapEvent");
  }

  core::events::EventSubscription<core::events::ViewportChangedEvent>
      viewportSub{
          .subscriber = "CameraSystem",
          .callback = [&](const core::events::ViewportChangedEvent &evt)
              -> std::expected<void, Error> {
            if (!evt.pScene) {
              FLOG_ERROR("ViewportChangedEvent received with nullptr scene");
              return {};
            }

            if (_pActiveCamera) {
              _pActiveCamera->UpdateProjection(evt.oNewWidth.value(),
                                               evt.oNewHeight.value());
              _pActiveCamera->viewportSize = core::math::Vec2(
                  evt.oNewWidth.value(), evt.oNewHeight.value());
              FLOG_INFO("CameraSystem: viewport updated");
            }
            return {};
          },
      };

  if (auto res = _eventBus.Subscribe(viewportSub); !res.has_value()) {
    FLOG_ERROR("failed to subscribe to ViewportChangedEvent");
  }
}

} // namespace febundle::systems
