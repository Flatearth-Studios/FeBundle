#ifndef INCLUDE_FEBUNDLE_CORE_GAME_BRIDGE_HPP_
#define INCLUDE_FEBUNDLE_CORE_GAME_BRIDGE_HPP_

#include "FeBundle/Core/Defines.hpp"
#include "FeBundle/Core/Events/EventBus.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include "FeBundle/Core/Scene/Scene.hpp"
#include "FeBundle/Core/Systems/RenderSystem.hpp"
#include "FeBundle/Core/Systems/UIManager.hpp"
#include <functional>

namespace febundle {

class GameBridge {
public:
  using CommandFn = std::function<void(void *)>;

  virtual ~GameBridge() = default;
  virtual void PostCommand(const string &name, void *payload) = 0;
  virtual void RenderScene(const scene::Scene &scene) = 0;
  virtual void LoadUIScene(const scene::Scene &scene) = 0;
  // virtual void Post(const string &name, std::any payload) = 0;
  // virtual void Enqueue(const string &name, std::function<void()> fn) = 0;
};

class GameBridgeImpl : public GameBridge {
public:
  explicit GameBridgeImpl(core::events::EventBus &evtBus,
                          systems::UIManager &uiManager);

  void PostCommand(const string &name, void *payload) override;
  void RenderScene(const scene::Scene &scene) override;
  void LoadUIScene(const scene::Scene &scene) override;
  // void Post(const string &name, std::any payload) override;
  // void Enqueue(const string &name, std::function<void()> fn) override;

private:
  core::events::EventBus &_eventBus;
  systems::RenderSystem _renderSystem;
};

} // namespace febundle

#endif // INCLUDE_FEBUNDLE_CORE_GAME_BRIDGE_HPP_
