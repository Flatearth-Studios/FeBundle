#include "FeBundle/Core/Window/Window.hpp"
#include "FeBundle/Core/Logger.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>

namespace febundle::window {

Window::Window(const WindowSpecs &specs) : _specs(specs) {
  if (specs.width == 0 || specs.height) {
    LOG_WARN("zero width/height on WindowSpecs");
  }
}

Window::~Window() {
  cleanup();
}

std::expected<void, Error> Window::Init() {
  if (_pWindow != nullptr) {
    LOG_WARN("attempt to initialize an window that is already initialized");
    return {};
  }
  SDL_Init(SDL_INIT_VIDEO);

  _pWindow = SDL_CreateWindow(_specs.title, _specs.width, _specs.height,
                             SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

  if (_pWindow == nullptr) {
    // Failed to create the window
    LOG_ERROR("failed to create window: {}", SDL_GetError());
    return std::unexpected{Error(ErrorName::CreateWindow)};
  }

  LOG_INFO("window initialized successfully");
  return {};
}

SDL_Window *Window::Handle() const noexcept {
  if (_pWindow == nullptr) {
    LOG_WARN("window handle is nullptr");
  }
  return _pWindow;
}

bool Window::ShouldClose() const noexcept {
  return _shouldClose;
}

void Window::PollEvents() {
  while (SDL_PollEvent(&_event)) {
    if (_event.type == SDL_EVENT_QUIT) {
      _shouldClose = true;
      break;
    }
  } 
}

void Window::cleanup() {
  _shouldClose = true;
  if (_pWindow == nullptr) {
    return;
  }

  SDL_DestroyWindow(_pWindow);
  SDL_Quit();
  _pWindow = nullptr;
}

} // namespace febundle::window
