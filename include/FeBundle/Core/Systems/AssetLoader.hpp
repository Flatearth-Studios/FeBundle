#ifndef INCLUDE_FEBUNDLE_CORE_SYSTEMS_ASSET_LOADER_HPP_
#define INCLUDE_FEBUNDLE_CORE_SYSTEMS_ASSET_LOADER_HPP_

#include "FeBundle/Core/Assets/Common.hpp"
#include "FeBundle/Core/Defines.hpp"
#include "FeBundle/Core/Scene/Entity.hpp"

namespace febundle::systems {

class AssetLoader {
public:
  assets::AssetHandle LoadFor(scene::Entity e, assets::AssetType assetType,
                              const fs::path &path);

  // void UnloadFor(scene::Entity e, const fs::path &path);
  // void UnloadAll(scene::Entity e);

  uint64 AssetCount() const;

  const umap<scene::Entity,
             uset<assets::AssetHandle, assets::AssetHandleHash>> &
  AllAssets() const;

  std::vector<assets::AssetHandle> DirtyAssets();

  bool IsDirty(const assets::AssetHandle &handle) const;

  uint64 Version() const;

  void MarkAsDirty(const fs::path &path);

private:
  std::atomic<uint64> _version{1};
  uint64 _nextAssetId{1};
  umap<scene::Entity, uset<assets::AssetHandle, assets::AssetHandleHash>>
      _mapOfAssetHandles;
  umap<fs::path, assets::AssetHandle> _pathToHandle;
  uset<assets::AssetHandle, assets::AssetHandleHash> _dirtyAssets;

  std::mutex _mtx;
};

} // namespace febundle::systems

#endif // INCLUDE_FEBUNDLE_CORE_SYSTEMS_ASSET_LOADER_HPP_
