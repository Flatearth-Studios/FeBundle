#include "FeBundle/Core/Events/GameCommandEvent.hpp"
#define FE_DEBUG
#include "FeBundle/Core/Application.hpp"
#include "FeBundle/Core/Events/AssetLoadEvent.hpp"
#include "FeBundle/Core/GameBridge.hpp"
#include "FeBundle/Core/Logger.hpp"
#include "FeBundle/Core/Memory/Memory.hpp"
#include "FeBundle/Core/Renderer/ImGuiLayer.hpp"
#include "FeBundle/Core/Renderer/Renderer.hpp"
#include "FeBundle/Core/Systems/InputManager.hpp"
#include <SDL3/SDL_events.h>

namespace febundle {

ApplicationState App::_appState{};

App::App(Game *gameInstance, bool logToFile, bool logToStdout)
    : _feWindow(gameInstance->windowSpecs), _assetManager(_eventBus),
      _uiManager(_eventBus),
      _pRenderer(std::move(MakeUnique<renderer::FeRenderer>(
                               memory::Tag::Renderer, _feWindow, _eventBus))
                     .value()),
      _audioSystem(_eventBus),
      _pBridge(std::move(memory::MakeUniquePoly<GameBridge, GameBridgeImpl>(
                             memory::Tag::Application, _eventBus, _uiManager))
                   .value()) {

  ENABLE_FILE_LOGGING(logToFile);
  _appState.gameInstance = gameInstance;
  _appState.gameInstance->pBridge = _pBridge.get();
}

App::~App() {
  FLOG_INFO("shutting down application");
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

  if (auto res = canRunGameInstance(); !res.has_value()) {
    FLOG_ERROR("game instance is malformed. Aborting");
    return std::unexpected{res.error()};
  }

  if (!_appState.gameInstance->Initialize(*_appState.gameInstance)) {
    FLOG_ERROR("could not initialize game instance");
    return std::unexpected{Error(ErrorName::InitializeGameCallback)};
  }
  _appState.gameInstance->isRunning = true;
  _appState.gameInstance->isSuspended = false;

  if (auto res = _pRenderer->Init(); !res.has_value()) {
    FLOG_ERROR("failed to initialize renderer");
    return std::unexpected{res.error()};
  }

  if (auto res = _audioSystem.Init(); !res.has_value()) {
    FLOG_ERROR("failed to initialize audio system");
    return std::unexpected{res.error()};
  }

  // Init ImGui
  _pImguiLayer =
      MakeUnique<renderer::ImGuiLayer>(memory::Tag::Renderer, _eventBus)
          .value();
  auto res = _pImguiLayer->Init(_feWindow.Handle(), _pRenderer->Handle());
  if (!res.has_value()) {
    FLOG_WARN("failed to initialize ImGui layer");
  }

  // --- Assets ---
  _assetManager.SetLoader(&_appState.gameInstance->assetLoader);
  _appState.width = _appState.gameInstance->windowSpecs.width;
  _appState.height = _appState.gameInstance->windowSpecs.height;

  // First load pass (registers all handles)
  _assetManager.Sync();
  FLOG_INFO("Initial asset sync complete");

  // Start background file watcher thread
  _assetManager.StartWatching();
  FLOG_INFO("FileWatcher is now monitoring assets/");

  // Start clock and finalize init
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
      for (auto &scene : _appState.gameInstance->scenes) {
        // TODO: rethink this loop
        scene.ProcessInputEvent(inputEvent);
      }
    }

    if (!_appState.gameInstance->isRunning) {
      break;
    }

    auto now = _appState.clock.NowTime();
    float64 deltaTime = now - _appState.lastTime;
    _appState.lastTime = now;

    _inputManager.Update();
    if (!_appState.gameInstance->Update(*_appState.gameInstance, deltaTime)) {
      FLOG_ERROR("game failed to update");
      break;
    }

    if (auto res = checkAndResizeWindow(); !res.has_value()) {
      FLOG_ERROR("failed to resize window");
      return std::unexpected{res.error()};
    }

    _assetManager.Sync();
    dispatchEvents();

    _pRenderer->BeginFrame();
    _pImguiLayer->BeginFrame();

    if (auto res = _pRenderer->Render(); !res.has_value()) {
      FLOG_ERROR("renderer failed to render");
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

std::expected<void, Error> App::canRunGameInstance() {
  if (!_appState.gameInstance->Initialize) {
    FLOG_ERROR("game function callback undefined: Initialize");
    return std::unexpected{Error(ErrorName::InitializeGameCallback)};
  }

  if (!_appState.gameInstance->Update) {
    FLOG_ERROR("game function callback undefined: Update");
    return std::unexpected{Error(ErrorName::InitializeGameCallback)};
  }

  if (!_appState.gameInstance->OnResize) {
    FLOG_ERROR("game function callback undefined: OnResize");
    return std::unexpected{Error(ErrorName::InitializeGameCallback)};
  }

  return {};
}

void App::dispatchEvents() {
  auto loadEvtRes = _eventBus.Dispatch<core::events::AssetLoadEvent>();
  if (!loadEvtRes.has_value()) {
    FLOG_WARN("failed to dispatch AssetLoadEvent for subscribers");
  }

  auto gameCmdEvtRes = _eventBus.Dispatch<core::events::GameCommandEvent>();
  if (!gameCmdEvtRes.has_value()) {
    FLOG_WARN("failed to dispatch GameCommandEvent for subscribers");
  }
}

} // namespace febundle
