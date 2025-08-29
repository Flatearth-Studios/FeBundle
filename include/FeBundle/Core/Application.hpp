#ifndef INCLUDE_FEBUNDLE_CORE_APPLICATION_HPP_
#define INCLUDE_FEBUNDLE_CORE_APPLICATION_HPP_

#include "Clock.hpp"
#include "Renderer/Renderer.hpp"
#include "GameTypes.hpp"
#include "Memory/Memory.hpp"

namespace febundle {

using RendererPtr = std::unique_ptr<renderer::IRenderer,
                                    memory::PolyDeleter<renderer::IRenderer>>;


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
  FEAPI App(Game *gameInstance);
  FEAPI std::expected<void, Error> Init();
  FEAPI std::expected<void, Error> Run();

private:
  static ApplicationState _appState;
  RendererPtr _pRenderer;
};

} // namespace febundle

#endif // INCLUDE_FEBUNDLE_CORE_APPLICATION_HPP_
