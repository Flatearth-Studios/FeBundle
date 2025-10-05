#define FE_DEBUG
#include "FeBundle/Core/Events/AssetLoadEvent.hpp"
#include "FeBundle/Core/Events/EventBus.hpp"
#include "FeBundle/Core/Assets/Common.hpp"
#include "FeBundle/Core/Assets/Texture.hpp"
#include "FeBundle/Core/Errors.hpp"
#include "FeBundle/Core/Logger.hpp"
#include "FeBundle/Core/Memory/Memory.hpp"
#include "FeBundle/Core/Systems/AssetLoader.hpp"
#include "FeBundle/Core/Systems/AssetManager.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>

namespace febundle::systems {

AssetManager::AssetManager(core::events::EventBus &evtBus) : _eventBus(evtBus) {
  _loaderImplementations.emplace(assets::AssetType::Texture, textureLoader);
}

AssetManager::~AssetManager() {
  auto &reg = registry();
  reg.assets.clear();
  reg.badAssets.clear();
  _fileWatcher.Relax();
  _pAssetLoader = nullptr;
}


void AssetManager::SetLoader(AssetLoader *pAl) { 
  _pAssetLoader = pAl; 
  _fileWatcher.SetLoader(pAl);
}

void AssetManager::StartWatching() {
  static bool watcherStarted = false;
  if (watcherStarted) {
    FLOG_INFO("FileWatcher already running");
    return;
  }

  if (_pAssetLoader == nullptr) {
    FLOG_WARN("cannot start FileWatcher: loader not set");
    return;
  }

  _fileWatcher.Watch("assets/");
  watcherStarted = true;
  FLOG_INFO("FileWatcher started");
}


void AssetManager::Sync() {
  if (_pAssetLoader == nullptr) {
    return;
  }

  firstRun();
  auto &reg = registry();
  if (_pAssetLoader->Version() == _latestVersion) {
    // No new assets to load
    return;
  }

  auto dirtyHandles = _pAssetLoader->DirtyAssets();
  for (const auto &handle : dirtyHandles) {
    const auto absPath = fs::absolute(handle.path);
    auto assetRes = loadImpl(absPath.string(), handle.type);
    if (!assetRes.has_value()) {
      FLOG_WARN("asset on path {} could not be loaded", absPath.string());
      reg.badAssets.insert(handle);
      continue;
    }

    reg.assets[handle] = std::move(*assetRes);
    reg.badAssets.erase(handle);

    core::events::AssetLoadEvent evt{
      .eventKind = core::events::AssetEventKind::Reload,
      .assetHandle = handle,
      .assetType = handle.type,
      .asset = reg.assets[handle].get(),
    };

    FLOG_INFO("firing new AssetLoadEvent");
    auto res = _eventBus.Push<core::events::AssetLoadEvent>(evt);
    FLOG_INFO("Asset {} reloaded", absPath.string());
  }

  _latestVersion = _pAssetLoader->Version();
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

void AssetManager::firstRun() {
  if (!_firstRun) {
    return;
  }

  auto &reg = registry();
  for (const auto &[entity, assetHandles] : _pAssetLoader->AllAssets()) {
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

      core::events::AssetLoadEvent evt{
          .eventKind = core::events::AssetEventKind::Load,
          .assetHandle = *it,
          .assetType = it->type,
          .asset = reg.assets[*it].get(),
      };

      FLOG_INFO("firing new AssetLoadEvent");
      auto res = _eventBus.Push<core::events::AssetLoadEvent>(evt);
      if (!res.has_value()) {
        FLOG_ERROR("failed to register AssetLoadEvent");
        continue;
      }

      FLOG_INFO("asset on path {} loaded successfuly", absPath.string());
    }
  }

  _latestVersion = _pAssetLoader->Version();
  _firstRun = false;
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
