#ifndef INCLUDE_FEBUNDLE_CORE_RENDERER_RENDERER_HPP_
#define INCLUDE_FEBUNDLE_CORE_RENDERER_RENDERER_HPP_

#include "../Window/Window.hpp"
#include "FeBundle/Core/Assets/Common.hpp"
#include "FeBundle/Core/Events/EventBus.hpp"
#include "FeBundle/Core/Math/Math.hpp"
#include "FeBundle/Core/Scene/Components.hpp"

#include <SDL3/SDL_render.h>

namespace febundle::renderer {

class FeRenderer {
public:
  explicit FeRenderer(window::Window &feWindow, core::events::EventBus &evtBus);
  ~FeRenderer();
  std::expected<void, Error> Init();
  std::expected<void, Error> Render();
  SDL_Renderer *Handle();
  void Resize(uint32 width, uint32 height);
  void BeginFrame();
  void EndFrame();

  void SetViewProjection(const core::math::Mat3 &view,
                         const core::math::Mat3 &projection);

private:
  std::expected<void, Error> initImGui();
  SDL_Texture *loadTexture(assets::AssetHandle ah);
  bool renderSprite(const scene::Transform &transform,
                    const scene::Sprite &sprite);

  void subscribeToEvents();
  void cleanup();

private:
  core::math::Mat3 _view{core::math::Mat3::Identity()};
  core::math::Mat3 _projection{core::math::Mat3::Identity()};
  SDL_Renderer *_pRenderer;
  window::Window &_feWindow;
  static bool _sInitialized;
  std::size_t _texturesLoaded{0};
  core::events::EventBus &_eventBus;
  umap<assets::AssetHandle, SDL_Texture *, assets::AssetHandleHash>
      _mapOfpTextures;
};

} // namespace febundle::renderer

#endif // INCLUDE_FEBUNDLE_CORE_RENDERER_RENDERER_HPP_
