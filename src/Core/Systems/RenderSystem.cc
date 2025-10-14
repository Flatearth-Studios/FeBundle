#include "FeBundle/Core/Systems/RenderSystem.hpp"
#include "FeBundle/Core/Events/EventBus.hpp"
#include "FeBundle/Core/Events/RenderEvents.hpp"
#include "FeBundle/Core/Logger.hpp"
#include "FeBundle/Core/Scene/Scene.hpp"

namespace febundle::systems {

RenderSystem::RenderSystem(core::events::EventBus &evtBus)
    : _eventBus(evtBus) {}

void RenderSystem::RenderScene(const scene::Scene &scene) {
  switch (scene.Type()) {
  case scene::SceneType::World: {
    renderWorld(scene);
    break;
  }
  case scene::SceneType::UI: {
    renderUI(scene);
    break;
  }
  }
}

void RenderSystem::renderWorld(const scene::Scene &scene) {
  using namespace scene;
  for (auto &[entity, comps] : scene.AccessAll()) {
    if (!comps.contains(scene::Component::Sprite) ||
        !comps.contains(scene::Component::Transform))
      continue;

    const auto *sprite = scene.GetComponent<scene::Sprite>(entity);
    const auto *transform = scene.GetComponent<scene::Transform>(entity);

    core::events::WorldRenderEvent evt(*sprite, *transform);

    if (auto res = _eventBus.Push(evt); !res.has_value()) {
      FLOG_ERROR("failed to register WorldRenderEvent");
    }
  }
}

void RenderSystem::renderUI(const scene::Scene &scene) {
  // TODO: implement UI system
}

} // namespace febundle::systems
