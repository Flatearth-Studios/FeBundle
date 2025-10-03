#ifndef INCLUDE_FEBUNDLE_CORE_RENDERER_RENDERER_HPP_
#define INCLUDE_FEBUNDLE_CORE_RENDERER_RENDERER_HPP_

#include "../Window/Window.hpp"
#include "FeBundle/Core/Assets/Common.hpp"
#include "FeBundle/Core/Assets/Texture.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include "FeBundle/Core/Scene/Scene.hpp"
#include "FeBundle/Core/Systems/AssetManager.hpp"

#include <SDL3/SDL_render.h>

namespace febundle::renderer {

class FeRenderer {
public:
  explicit FeRenderer(window::Window &feWindow, systems::AssetManager &am);
  ~FeRenderer();
  std::expected<void, Error> Init();
  std::expected<void, Error> Render();
  SDL_Renderer *Handle();
  void SetScene(const scene::Scene *scene);
  void Resize(uint32 width, uint32 height);
  void BeginFrame();
  void EndFrame();

private:
  std::expected<void, Error> initImGui();
  SDL_Texture *loadTexture(assets::AssetHandle ah);
  bool renderSprite(const scene::Transform &transform,
                    const scene::Sprite &sprite);

  void cleanup();

private:
  SDL_Renderer *_pRenderer;
  window::Window &_feWindow;
  const scene::Scene *_cpScene;
  static bool _sInitialized;
  std::size_t _texturesLoaded{0};
  systems::AssetManager &_assetManager;
  umap<assets::AssetHandle, SDL_Texture *, assets::AssetHandleHash>
      _mapOfpTextures;
};

} // namespace febundle::renderer

#endif // INCLUDE_FEBUNDLE_CORE_RENDERER_RENDERER_HPP_
