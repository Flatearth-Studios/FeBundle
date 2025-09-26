#ifndef INCLUDE_FEBUNDLE_CORE_APPLICATION_HPP_
#define INCLUDE_FEBUNDLE_CORE_APPLICATION_HPP_

#include "Clock.hpp"
#include "FeBundle/Core/Renderer/ImGuiLayer.hpp"
#include "FeBundle/Core/Systems/CollisionSystem.hpp"
#include "FeBundle/Core/Systems/InputManager.hpp"
#include "GameTypes.hpp"
#include "Memory/Memory.hpp"
#include "Renderer/Renderer.hpp"

namespace febundle {

using RendererPtr = std::unique_ptr<renderer::FeRenderer,
                                    memory::Deleter<renderer::FeRenderer>>;

using ImGuiPtr = std::unique_ptr<renderer::ImGuiLayer,
                                 memory::Deleter<renderer::ImGuiLayer>>;

struct ApplicationState {
  Game *gameInstance;
  bool isRunning, isSuspended;
  uint32 width, height;
  float64 lastTime;
  Clock clock;

  memory::MemoryManager &_mm = memory::MemoryManager::Self();
};

class App {
public:
  FEAPI App(Game *gameInstance, bool logToFile = true, bool logToStdout = true);
  FEAPI ~App();
  FEAPI std::expected<void, Error> Init();
  FEAPI std::expected<void, Error> Run();

private:
  std::expected<void, Error> checkAndResizeWindow();

private:
  static ApplicationState _appState;
  RendererPtr _pRenderer;
  ImGuiPtr _pImguiLayer;
  systems::InputManager _inputManager;
  window::Window _feWindow;
  systems::CollisionSystem _collisionSys;
  std::size_t _previousSceneIndex;
};

} // namespace febundle

#endif // INCLUDE_FEBUNDLE_CORE_APPLICATION_HPP_
