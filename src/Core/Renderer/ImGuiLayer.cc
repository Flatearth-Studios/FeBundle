#define FE_DEBUG
#include "FeBundle/Core/Renderer/ImGuiLayer.hpp"
#include "FeBundle/Core/Events/EventBus.hpp"
#include "FeBundle/Core/Events/EventQueue.hpp"
#include "FeBundle/Core/Events/RenderEvents.hpp"
#include "FeBundle/Core/Logger.hpp"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <SDL3/SDL_video.h>

namespace febundle::renderer {

ImGuiLayer::ImGuiLayer(core::events::EventBus &evtBus) : _eventBus(evtBus) {}

std::expected<void, Error> ImGuiLayer::Init(SDL_Window *window,
                                            SDL_Renderer *renderer) {
  if (_initialized) {
    FLOG_WARN("ImGuiLayer already initialized");
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
    FLOG_ERROR("failed to initialized SDL3 ImGui bridge");
    return std::unexpected{Error(ErrorName::ImGuiBackendInit)};
  }

  if (!ImGui_ImplSDLRenderer3_Init(renderer)) {
    FLOG_ERROR("failed to initialize SDLRenderer3 ImGui bridge");
    return std::unexpected{Error(ErrorName::ImGuiBackendInit)};
  }

  core::events::EventSubscription<core::events::UIRenderEvent> subscription{
      .subscriber = "ImGuiLayer",
      .callback = [&](const core::events::UIRenderEvent &evt)
          -> std::expected<void, Error> {
        evt.drawFn();
        return {};
      },
  };

  if (auto res = _eventBus.Subscribe(subscription); !res.has_value()) {
    FLOG_ERROR("failed to subscribe to UIRenderEvent");
  }

  _initialized = true;
  _pRenderer = renderer;
  return {};
}

void ImGuiLayer::BeginFrame() {
  if (!_initialized) {
    FLOG_WARN("cannot begin frame on unintialized ImGui layer");
    return;
  }

  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
}

void ImGuiLayer::Render() {
  if (!_initialized || _pRenderer == nullptr) {
    FLOG_WARN("attempt to render ImGui layer but either no renderer exists or "
              "layer is not initialized");
    return;
  }

  const ImGuiViewport *vp = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(vp->Pos);
  ImGui::SetNextWindowSize(vp->Size);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);

  ImGui::Begin("FeBundle_UIRoot", nullptr,
               ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                   ImGuiWindowFlags_NoBringToFrontOnFocus);

  if (auto res = _eventBus.Dispatch<core::events::UIRenderEvent>();
      !res.has_value()) {
    FLOG_ERROR("failed to dispatch UIRenderEvents");
  }

  ImGui::End();
  ImGui::PopStyleVar(3);

  // Optionally still draw your debug overlay
  ImGui::Begin("FeBundle Stats");
  ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
  ImGui::End();

  ImGui::Render();
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), _pRenderer);
}

void ImGuiLayer::Shutdown() {
  if (!_initialized) {
    FLOG_WARN("cannot shutdown unitialized ImGui layer");
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
