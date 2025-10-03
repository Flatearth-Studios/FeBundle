#include "FeBundle/Core/Systems/AssetManager.hpp"
#include "FeBundle/Core/Assets/Common.hpp"
#include "FeBundle/Core/Assets/Texture.hpp"
#include "FeBundle/Core/Errors.hpp"
#include "FeBundle/Core/Logger.hpp"
#include "FeBundle/Core/Memory/Memory.hpp"
#include "FeBundle/Core/Systems/AssetLoader.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>

namespace febundle::systems {

AssetManager::~AssetManager() { 
  auto &reg = registry();
  reg.assets.clear();
  reg.badAssets.clear();
  _cpAssetLoader = nullptr; 
}

void AssetManager::SetLoader(const AssetLoader *cpAl) {
  _cpAssetLoader = cpAl;
}

void AssetManager::Sync() {
  if (_cpAssetLoader == nullptr) {
    return;
  }

  auto &reg = registry();
  std::size_t assetCount = reg.assets.size() + reg.badAssets.size();
  if (_cpAssetLoader->Version() == _latestVersion) {
    // No new assets to load
    return;
  }

  for (const auto &[entity, assetHandles] : _cpAssetLoader->AllAssets()) {
    for (auto it = assetHandles.begin(); it != assetHandles.end(); it++) {
      if (reg.assets.contains(*it)) {
        continue;
      }

      const auto absPath = fs::absolute(it->path);
      auto assetRes = loadImpl(absPath.string());
      if (!assetRes.has_value()) {
        reg.badAssets.insert(*it);
        continue;
      }

      reg.assets.emplace(*it, std::move(assetRes.value()));
      if (reg.badAssets.contains(*it)) {
        reg.badAssets.erase(*it);
      }
    }
  }

  _latestVersion = _cpAssetLoader->Version();
}

assets::IAsset *AssetManager::AssetOf(const assets::AssetHandle &ah) {
  auto &reg = registry();
  if (!reg.assets.contains(ah)) {
    return nullptr;
  }

  return reg.assets.at(ah).get();
}

std::expected<IAssetPtr, Error> AssetManager::loadImpl(const string &path) {
  SDL_Surface *surf = IMG_Load(path.c_str());
  if (surf == nullptr) {
    FLOG_ERROR("failed to load image {}: {}", path, SDL_GetError());
    return std::unexpected{Error(ErrorName{ErrorName::LoadImage})};
  }

  auto res = memory::MakeUniquePoly<assets::IAsset, assets::Texture>(
      memory::Tag::AssetManager, surf);

  if (!res.has_value()) {
    FLOG_ERROR("failed to allocate Texture for '{}'", path);
    SDL_DestroySurface(surf);
    return std::unexpected{res.error()};
  }

  return std::move(res.value());
}

} // namespace febundle::systems
