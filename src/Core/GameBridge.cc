#define FE_DEBUG
#include "FeBundle/Core/Events/SceneLoadedEvent.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include "FeBundle/Core/Scene/Scene.hpp"
#include "FeBundle/Core/Systems/UIManager.hpp"
#include "FeBundle/Core/Events/EventBus.hpp"
#include "FeBundle/Core/Events/GameCommandEvent.hpp"
#include "FeBundle/Core/GameBridge.hpp"
#include "FeBundle/Core/Logger.hpp"

namespace febundle {

GameBridgeImpl::GameBridgeImpl(core::events::EventBus &evtBus,
                               systems::UIManager &uiManager)
    : _eventBus(evtBus), _renderSystem(evtBus, uiManager) {}

void GameBridgeImpl::PostCommand(const string &name, void *payload) {
  core::events::GameCommandEvent evt{
      .name = name,
      .payload = payload,
  };

  auto res = _eventBus.Push<core::events::GameCommandEvent>(evt);
  if (!res.has_value()) {
    FLOG_WARN("failed to push game command '{}'", name);
  }
}

void GameBridgeImpl::RenderScene(const scene::Scene &scene) {
  _renderSystem.RenderScene(scene);
}

void GameBridgeImpl::LoadScene(const scene::Scene &scene) {
  core::events::SceneLoadedEvent evt{
      .cpScene = &scene,
  };

  if (auto res = _eventBus.Push(evt); !res.has_value()) {
    FLOG_ERROR("failed to push SceneLoadedEvent");
    return;
  }

  if (auto res = _eventBus.Dispatch<core::events::SceneLoadedEvent>();
      !res.has_value()) {
    FLOG_ERROR("failed to dispatch SceneLoadedEvents");
    return;
  }
}

} // namespace febundle
