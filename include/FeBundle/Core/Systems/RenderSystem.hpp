#ifndef INCLUDE_FEBUNDLE_CORE_SYSTEMS_RENDER_SYSTEM_HPP_
#define INCLUDE_FEBUNDLE_CORE_SYSTEMS_RENDER_SYSTEM_HPP_

#include "FeBundle/Core/Events/EventBus.hpp"
#include "FeBundle/Core/Scene/Scene.hpp"
namespace febundle::systems {

class RenderSystem {
public:
  explicit RenderSystem(core::events::EventBus &evtBus);
  
  void RenderScene(const scene::Scene &scene);

private:
  void renderWorld(const scene::Scene &scene);
  void renderUI(const scene::Scene &scene);

private:
  core::events::EventBus &_eventBus;

};

}

#endif // INCLUDE_FEBUNDLE_CORE_SYSTEMS_RENDER_SYSTEM_HPP_
