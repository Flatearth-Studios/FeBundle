#include "FeBundle/Core/Systems/InputManager.hpp"
#define FE_DEBUG
#include "FeBundle/Core/Application.hpp"
#include "FeBundle/Core/Logger.hpp"
#include "FeBundle/Core/Memory/Memory.hpp"
#include "FeBundle/Core/Renderer/ImGuiLayer.hpp"
#include "FeBundle/Core/Renderer/Renderer.hpp"
#include <SDL3/SDL_events.h>

namespace febundle {

ApplicationState App::_appState{};

App::App(Game *gameInstance, bool logToFile, bool logToStdout)
    : _feWindow(gameInstance->windowSpecs),
      _pRenderer(std::move(MakeUnique<renderer::FeRenderer>(
                               memory::Tag::Renderer, _feWindow))
                     .value()) {

  ENABLE_FILE_LOGGING(logToFile);
  ENABLE_STDOUT_LOGGING(logToStdout);
  _appState.gameInstance = gameInstance;
}

App::~App() {
  FLOG_TRACE("shutting down application");
  if (_pImguiLayer != nullptr) {
    _pImguiLayer->Shutdown();
    _pImguiLayer.reset();
  }
}

std::expected<void, Error> App::Init() {
  if (auto res = _feWindow.Init(); !res.has_value()) {
    FLOG_ERROR("failed to initialize window");
    return std::unexpected{res.error()};
  }

  if (!_appState.gameInstance->Initialize(*_appState.gameInstance)) {
    FLOG_ERROR("could not initialize game instance");
    return std::unexpected{Error(ErrorName::InitializeGameCallback)};
  }

  if (auto res = _pRenderer->Init(); !res.has_value()) {
    FLOG_ERROR("failed to initialize renderer");
    return std::unexpected{res.error()};
  }
  // Sets the scene for the renderer to render
  _pRenderer->SetScene(&_appState.gameInstance
                            ->scenes[_appState.gameInstance->activeSceneIndex]);

  _previousSceneIndex = _appState.gameInstance->activeSceneIndex;
  _pImguiLayer =
      MakeUnique<renderer::ImGuiLayer>(memory::Tag::Renderer).value();
  auto res = _pImguiLayer->Init(_feWindow.Handle(), _pRenderer->Handle());
  if (!res.has_value()) {
    FLOG_WARN("failed to initialize ImGui layer");
  }

  _appState.width = _appState.gameInstance->windowSpecs.width;
  _appState.height = _appState.gameInstance->windowSpecs.height;
  _appState.clock.Start();
  FLOG_INFO("application initialized successfully");
  return {};
}

std::expected<void, Error> App::Run() {
  FLOG_TRACE("starting application");

  _appState.clock.Update();
  _appState.lastTime = _appState.clock.NowTime();

  while (!_feWindow.ShouldClose()) {
    SDL_Event event;
    systems::InputEvent *inputEvent;
    while (SDL_PollEvent(&event)) {
      _feWindow.ProcessEvent(event);
      _pImguiLayer->ProcessEvent(event);
      inputEvent = _inputManager.ProcessEvent(event);
      _appState.gameInstance->scenes[_appState.gameInstance->activeSceneIndex]
          .ProcessInputEvent(inputEvent);
    }

    auto now = _appState.clock.NowTime();
    float64 deltaTime = now - _appState.lastTime;
    _appState.lastTime = now;

    _inputManager.Update();
    if (!_appState.gameInstance->Update(*_appState.gameInstance, deltaTime)) {
      FLOG_ERROR("game failed to update");
      break;
    }

    if (_appState.gameInstance->activeSceneIndex != _previousSceneIndex) {
      _previousSceneIndex = _appState.gameInstance->activeSceneIndex;
      _pRenderer->SetScene(
          &_appState.gameInstance->scenes[_previousSceneIndex]);
      FLOG_INFO("Renderer rebound to scene {}", _previousSceneIndex);
    }

    if (auto res = checkAndResizeWindow(); !res.has_value()) {
      FLOG_ERROR("failed to resize window");
      return std::unexpected{res.error()};
    }

    _pRenderer->BeginFrame();
    _pImguiLayer->BeginFrame();

    if (auto res = _pRenderer->Render(); !res.has_value()) {
      FLOG_ERROR("renderer failed to render scene");
      return std::unexpected{res.error()};
    }

    _pImguiLayer->Render();
    _pRenderer->EndFrame();
  }

  return {};
}

std::expected<void, Error> App::checkAndResizeWindow() {
  const auto newSpecs = _feWindow.Specs();
  bool resized = false;
  if (newSpecs.height != _appState.height ||
      newSpecs.width != _appState.width) {
    _appState.height = newSpecs.height;
    _appState.width = newSpecs.width;
    _appState.gameInstance->windowSpecs = newSpecs;
    _pRenderer->Resize(_appState.width, _appState.height);
    resized = true;
  }

  if (resized &&
      !_appState.gameInstance->OnResize(*_appState.gameInstance,
                                        _appState.width, _appState.height)) {
    FLOG_ERROR("game failed to resize");
    return std::unexpected{Error(ErrorName::ResizeWindow)};
  }

  return {};
}

} // namespace febundle
