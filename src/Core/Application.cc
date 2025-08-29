#include "FeBundle/Core/Application.hpp"
#include "FeBundle/Core/Logger.hpp"
#include "FeBundle/Core/Memory/Memory.hpp"
#include "FeBundle/Core/Renderer/Renderer.hpp"

namespace febundle {

using memory::MakeUniquePoly;

ApplicationState App::_appState{};

App::App(Game *gameInstance)
    : _pRenderer(
          std::move(MakeUniquePoly<renderer::IRenderer, renderer::FeRenderer>(
                        memory::Tag::Renderer, gameInstance->windowSpecs))
              .value()) {
  _appState.gameInstance = gameInstance;
  LOG_INFO("application initalized successfully");
}

std::expected<void, Error> App::Init() {
  if (auto res = _pRenderer->Init(); !res.has_value()) {
    LOG_ERROR("failed to initialize renderer");
    return std::unexpected{res.error()};
  }

  _appState.width = _appState.gameInstance->windowSpecs.width;
  _appState.height = _appState.gameInstance->windowSpecs.height;
  _appState.clock.Start();
  LOG_INFO("application initialized successfully");
  return {};
}

std::expected<void, Error> App::Run() {
  auto res = _pRenderer->Render();
  if (!res.has_value()) {
    LOG_ERROR("failed to render frame");
    return std::unexpected{res.error()};
  }

  return {};
}

} // namespace febundle
