#ifndef INCLUDE_FEBUNDLE_CORE_WINDOW_WINDOW_HPP_
#define INCLUDE_FEBUNDLE_CORE_WINDOW_WINDOW_HPP_

#include "../Defines.hpp"
#include <SDL3/SDL_events.h>

namespace febundle::window {

struct WindowSpecs {
  uint32 width{}, height{};
  const char *title;

  WindowSpecs() = default;
  WindowSpecs(const char *title) : title(title) {}
};

class Window {
public:
  Window();
  Window(const WindowSpecs &specs);
  ~Window();
  std::expected<void, Error> Init();
  SDL_Window *Handle() const noexcept;
  bool ShouldClose() const noexcept;
  void PollEvents();

private:
  void cleanup();

private:
  bool _shouldClose{false};
  SDL_Event _event;
  WindowSpecs _specs;
  SDL_Window *_pWindow;
};

}

#endif
