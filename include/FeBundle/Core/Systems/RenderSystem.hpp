#ifndef INCLUDE_FEBUNDLE_CORE_SYSTEMS_RENDER_SYSTEM_HPP_
#define INCLUDE_FEBUNDLE_CORE_SYSTEMS_RENDER_SYSTEM_HPP_

#include "FeBundle/Core/Events/EventBus.hpp"
#include "FeBundle/Core/Scene/Scene.hpp"
#include "FeBundle/Core/Systems/UIManager.hpp"
namespace febundle::systems {

class RenderSystem {
public:
  explicit RenderSystem(core::events::EventBus &evtBus, UIManager &uiManager);

  void RenderScene(const scene::Scene &scene);

private:
  void registerUIElements(const scene::Scene &scene);
  void renderWorld(const scene::Scene &scene);
  void renderUI(const scene::Scene &scene);

private:
  core::events::EventBus &_eventBus;
  UIManager &_uiManager;
};

} // namespace febundle::systems

#endif // INCLUDE_FEBUNDLE_CORE_SYSTEMS_RENDER_SYSTEM_HPP_
