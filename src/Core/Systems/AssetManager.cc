#define FE_DEBUG
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

AssetManager::AssetManager() {
  _loaderImplementations.emplace(assets::AssetType::Texture, textureLoader);
}

AssetManager::~AssetManager() {
  auto &reg = registry();
  reg.assets.clear();
  reg.badAssets.clear();
  _cpAssetLoader = nullptr;
}

void AssetManager::SetLoader(const AssetLoader *cpAl) { _cpAssetLoader = cpAl; }

void AssetManager::Sync() {
  if (_cpAssetLoader == nullptr) {
    return;
  }

  auto &reg = registry();
  std::size_t assetCount = reg.assets.size() + reg.badAssets.size();
  if (_cpAssetLoader->Version() == _latestVersion) {
    FLOG_DEBUG("already on the latest version of asset loader");
    // No new assets to load
    return;
  }

  for (const auto &[entity, assetHandles] : _cpAssetLoader->AllAssets()) {
    for (auto it = assetHandles.begin(); it != assetHandles.end(); it++) {
      if (reg.assets.contains(*it)) {
        continue;
      }

      const auto absPath = fs::absolute(it->path);
      auto assetRes = loadImpl(absPath.string(), it->type);
      if (!assetRes.has_value()) {
        FLOG_WARN("asset on path {} could not be loaded", absPath.string());
        reg.badAssets.insert(*it);
        continue;
      }

      reg.assets.emplace(*it, std::move(assetRes.value()));
      if (reg.badAssets.contains(*it)) {
        reg.badAssets.erase(*it);
      }
      FLOG_INFO("asset on path {} loaded successfuly", absPath.string());
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

std::expected<IAssetPtr, Error> AssetManager::loadImpl(const string &path,
                                                       assets::AssetType type) {
  if (!_loaderImplementations.contains(type))
    return std::unexpected{Error(ErrorName::UnknownAsset)};
  return _loaderImplementations[type](path);
}

// LOADERS

std::expected<IAssetPtr, Error>
AssetManager::textureLoader(const string &path) {
  SDL_Surface *surf = IMG_Load(path.c_str());
  if (!surf) {
    return std::unexpected{Error(ErrorName::LoadImage)};
  }

  return memory::MakeUniquePoly<assets::IAsset, assets::Texture>(
      memory::Tag::AssetManager, surf);
}

} // namespace febundle::systems
