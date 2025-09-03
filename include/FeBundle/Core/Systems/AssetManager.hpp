#ifndef INCLUDE_CORE_SYSTEMS_ASSET_MANAGER_HPP_
#define INCLUDE_CORE_SYSTEMS_ASSET_MANAGER_HPP_

#include "FeBundle/Core/Scene/Components.hpp"
#include <SDL3/SDL_render.h>

namespace febundle::renderer {
class FeRenderer;
}

namespace febundle::systems {

class AssetManager {
public:
  void RegisterTexture(scene::TextureHandle tex, const string &path);
  std::size_t TextureQty() const;

private:
  friend renderer::FeRenderer;
  void unloadAll(SDL_Renderer *renderer);
  SDL_Texture *texture(scene::TextureHandle tex);
  SDL_Texture *loadTexture(SDL_Renderer *renderer, scene::TextureHandle tex);

private:
  uset<scene::TextureHandle> _setOfTextureHandles;
  umap<scene::TextureHandle, string> _mapOfPaths;
  umap<scene::TextureHandle, SDL_Texture *> _mapOfpTextures;
};

}

#endif // INCLUDE_CORE_SYSTEMS_ASSET_MANAGER_HPP_
