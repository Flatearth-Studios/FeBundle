#define FE_DEBUG
#include "FeBundle/Core/Window/Window.hpp"
#include "FeBundle/Core/Logger.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>

namespace febundle::window {

Window::Window(const WindowSpecs &specs) : _specs(specs) {
  if (_specs.width == 0 || _specs.height == 0) {
    FLOG_WARN("zero width/height on WindowSpecs");
  }
}

Window::~Window() { cleanup(); }

std::expected<void, Error> Window::Init() {
  if (_pWindow != nullptr || _initialized) {
    FLOG_WARN("attempt to initialize an window that is already initialized");
    return std::unexpected{ Error(ErrorName::CreateWindow) };
  }
  SDL_Init(SDL_INIT_VIDEO);

  _pWindow = SDL_CreateWindow(_specs.title, _specs.width, _specs.height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

  if (_pWindow == nullptr) {
    // Failed to create the window
    FLOG_ERROR("failed to create window: {}", SDL_GetError());
    return std::unexpected{Error(ErrorName::CreateWindow)};
  }

  FLOG_INFO("window initialized successfully");
  FLOG_INFO("Specs {}x{}", _specs.width, _specs.height);
  _initialized = true;
  return {};
}

const WindowSpecs &Window::Specs() const {
  return _specs;
}

SDL_Window *Window::Handle() const noexcept {
  if (_pWindow == nullptr) {
    LOG_WARN("window handle is nullptr");
  }
  return _pWindow;
}

bool Window::ShouldClose() const noexcept { return _shouldClose; }

void Window::ProcessEvent(const SDL_Event &event) {
  bool resized = false;
  uint32 newW = 0, newH = 0;
  switch (event.type) {
  case SDL_EVENT_QUIT:
    _shouldClose = true;
    break;
  case SDL_EVENT_WINDOW_RESIZED: {
    resized = true; 
    newW = static_cast<uint32>(event.window.data1);
    newH = static_cast<uint32>(event.window.data2);
    break;
  }
  default:
    break;
  }

  if (resized && (newW != _specs.width || newH != _specs.height)) {
    _specs.width = newW;
    _specs.height = newH;
    FLOG_TRACE("resizing...");
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
  _initialized = false;
}

} // namespace febundle::window
