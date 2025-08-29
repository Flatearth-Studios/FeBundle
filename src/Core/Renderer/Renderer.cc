#include "FeBundle/Core/Renderer/Renderer.hpp"
#include "FeBundle/Core/Logger.hpp"
#include "FeBundle/Core/Window/Window.hpp"
#include <SDL3/SDL_render.h>

namespace febundle::renderer {

FeRenderer::FeRenderer(const window::WindowSpecs &specs)
    : _feWindow(specs) {}

FeRenderer::~FeRenderer() {
  cleanup();
}


std::expected<void, Error> FeRenderer::Init() {
  if (auto winInit = _feWindow.Init(); !winInit.has_value()) {
    LOG_ERROR("failed to create window");
    return std::unexpected{winInit.error()};
  }

  SDL_Window* window = _feWindow.Handle();
  _pRenderer = SDL_CreateRenderer(window, nullptr);
  if (_pRenderer == nullptr) {
    LOG_ERROR("failed to create renderer: {}", SDL_GetError());
    return std::unexpected{Error(ErrorName::CreateRenderer)};
  }

  LOG_INFO("renderer initialized successfully");
  return {};
}


std::expected<void, Error> FeRenderer::Render() {
  if (_pRenderer == nullptr) {
    LOG_ERROR("attempt to render on nullptr");
    return std::unexpected{Error(ErrorName::RenderCall)};
  }

  while (!_feWindow.ShouldClose()) {
    _feWindow.PollEvents();
    SDL_RenderClear(_pRenderer);
    SDL_RenderPresent(_pRenderer);
  }

  return {};
}

void FeRenderer::Resize(uint32 width, uint32 height) {}

void FeRenderer::BeginFrame() {}

void FeRenderer::EndFrame() {}

void FeRenderer::cleanup() {
  if (_pRenderer == nullptr) {
    return;
  }

  SDL_DestroyRenderer(_pRenderer); 
  _pRenderer = nullptr;
}

} // namespace febundle::renderer
