#ifndef INCLUDE_FEBUNDLE_CORE_RENDERER_RENDERER_HPP_
#define INCLUDE_FEBUNDLE_CORE_RENDERER_RENDERER_HPP_

#include "../Window/Window.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include "FeBundle/Core/Scene/Scene.hpp"

#include <SDL3/SDL_render.h>

namespace febundle::renderer {

class FeRenderer {
public:
  explicit FeRenderer(window::Window &feWindow);
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
  bool renderSprite(const scene::Transform &transform,
                    const scene::Sprite &sprite, const scene::Texture &texture);
  SDL_Texture *loadTexture(const scene::Texture &texture);
  std::size_t loadTextures();
  void cleanup();

private:
  SDL_Renderer *_pRenderer;
  window::Window &_feWindow;
  const scene::Scene *_cpScene;
  static bool _sInitialized;
  std::size_t _texturesLoaded{0};
  umap<scene::TextureHandle, SDL_Texture *> _mapOfpTextures;
};

} // namespace febundle::renderer

#endif // INCLUDE_FEBUNDLE_CORE_RENDERER_RENDERER_HPP_
