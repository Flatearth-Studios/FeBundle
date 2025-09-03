#include "FeBundle/Core/Systems/AssetManager.hpp"
#include "FeBundle/Core/Logger.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>

namespace febundle::systems {

void AssetManager::RegisterTexture(scene::TextureHandle tex,
                                   const string &path) {
  _mapOfPaths.emplace(tex, path);
}

std::size_t AssetManager::TextureQty() const {
  return _setOfTextureHandles.size();
}

void AssetManager::unloadAll(SDL_Renderer *renderer) {
  for (const auto [tex, path] : _mapOfPaths) {
    if (_mapOfpTextures.contains(tex)) {
      continue;
    }

    auto texture = loadTexture(renderer, tex);
    if (texture == nullptr) {
      FLOG_WARN("loaded texture '{}' is a nullptr", tex);
    }
  }
}

SDL_Texture *AssetManager::texture(scene::TextureHandle tex) {
  auto it = _mapOfpTextures.find(tex);
  return it != _mapOfpTextures.end() ? it->second : nullptr;
}

SDL_Texture *AssetManager::loadTexture(SDL_Renderer *renderer,
                                       scene::TextureHandle tex) {
  auto texIt = _mapOfpTextures.find(tex);
  if (texIt != _mapOfpTextures.end()) {
    return texIt->second;
  }

  auto it = _mapOfPaths.find(tex);
  if (it == _mapOfPaths.end())
    return nullptr;

  const string &path = it->second;

  // use SDL_image, not SDL_LoadBMP

  SDL_Texture *texture = IMG_LoadTexture(renderer, path.c_str());
  if (!texture) {
    FLOG_ERROR("Failed to load texture {}: {}", path.c_str(), SDL_GetError());
    return nullptr;
  }

  _mapOfpTextures[tex] = texture;
  return texture;
}

} // namespace febundle::systems
