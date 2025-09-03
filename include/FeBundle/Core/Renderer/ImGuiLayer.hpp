#ifndef INCLUDE_FEBUNDLE_CORE_RENDERER_IMGUI_LAYER_HPP_
#define INCLUDE_FEBUNDLE_CORE_RENDERER_IMGUI_LAYER_HPP_

#include "FeBundle/Core/Errors.hpp"
#include "FeBundle/Core/Defines.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>

namespace febundle::renderer {

class ImGuiLayer {
public:
  std::expected<void, Error> Init(SDL_Window *window, SDL_Renderer *renderer);
  void BeginFrame();
  void Render();
  void Shutdown();
  bool ProcessEvent(const SDL_Event &e);
private:
  bool _initialized{false};
  SDL_Renderer *_pRenderer;
};
}

#endif // INCLUDE_FEBUNDLE_CORE_RENDERER_IMGUI_LAYER_HPP_
