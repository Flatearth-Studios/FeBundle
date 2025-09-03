#define FE_DEBUG
#include "FeBundle/Core/Renderer/ImGuiLayer.hpp"
#include "FeBundle/Core/Logger.hpp"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <SDL3/SDL_video.h>

namespace febundle::renderer {

std::expected<void, Error> ImGuiLayer::Init(SDL_Window *window,
                                            SDL_Renderer *renderer) {
  if (_initialized) {
    LOG_WARN("ImGuiLayer already initialized");
    return {};
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_NavEnableKeyboard;

  float32 scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  ImGui::GetStyle().ScaleAllSizes(scale);
  io.FontGlobalScale = scale;
  io.Fonts->AddFontDefault();

  if (!ImGui_ImplSDL3_InitForSDLRenderer(window, renderer)) {
    LOG_ERROR("failed to initialized SDL3 ImGui bridge");
    return std::unexpected{Error(ErrorName::ImGuiBackendInit)};
  }

  if (!ImGui_ImplSDLRenderer3_Init(renderer)) {
    LOG_ERROR("failed to initialize SDLRenderer3 ImGui bridge");
    return std::unexpected{Error(ErrorName::ImGuiBackendInit)};
  }

  _initialized = true;
  _pRenderer = renderer;
  return {};
}

void ImGuiLayer::BeginFrame() {
  if (!_initialized) {
    LOG_WARN("cannot begin frame on unintialized ImGui layer");
    return;
  }

  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
}

void ImGuiLayer::Render() {
  if (!_initialized || _pRenderer == nullptr) {
    LOG_WARN("attempt to render ImGui layer but either no renderer exists or "
             "layer is not initialized");
    return;
  }

  ImGui::Begin("FeBundle Stats");
  ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
  ImGui::End();

  ImGui::Render();
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), _pRenderer);
}

void ImGuiLayer::Shutdown() {
  if (!_initialized) {
    LOG_WARN("cannot shutdown unitialized ImGui layer");
    return;
  }
}

bool ImGuiLayer::ProcessEvent(const SDL_Event &e) {
  if (!_initialized) {
    return false;
  }
  return ImGui_ImplSDL3_ProcessEvent(&e);
}

} // namespace febundle::renderer
