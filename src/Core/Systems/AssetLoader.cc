#include "FeBundle/Core/Systems/AssetLoader.hpp"
#include "FeBundle/Core/Assets/Common.hpp"

namespace febundle::systems {

assets::AssetHandle AssetLoader::LoadFor(scene::Entity e,
                                         assets::AssetType assetType,
                                         const fs::path &path) {
  // Candidate handle at last allocated id
  assets::AssetHandle retHandle(_nextAssetId - 1, assetType, path);

  auto &assetHandles = _mapOfAssetHandles[e]; // auto-creates if missing

  // If identical handle exists → return it
  if (assetHandles.contains(retHandle)) {
    _nextAssetId++; // still advance counter
    return retHandle;
  }

  // Otherwise, assign a new id (monotonic)
  retHandle.id++;
  assetHandles.insert(retHandle);

  _nextAssetId++;
  _version++;
  return retHandle;
}

uint64 AssetLoader::AssetCount() const { return _nextAssetId - 1; }

const umap<scene::Entity, uset<assets::AssetHandle, assets::AssetHandleHash>> &
AssetLoader::AllAssets() const {
  return _mapOfAssetHandles;
}

uint64 AssetLoader::Version() const { return _version; }

} // namespace febundle::systems
