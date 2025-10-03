#ifndef INCLUDE_FEBUNDLE_CORE_SYSTEMS_ASSET_MANAGER_HPP_
#define INCLUDE_FEBUNDLE_CORE_SYSTEMS_ASSET_MANAGER_HPP_

#include "FeBundle/Core/Assets/Common.hpp"
#include "FeBundle/Core/Defines.hpp"
#include "FeBundle/Core/Memory/Memory.hpp"
#include "FeBundle/Core/Systems/AssetLoader.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>

namespace febundle::systems {

using IAssetPtr =
    std::unique_ptr<assets::IAsset, memory::PolyDeleter<assets::IAsset>>;

class AssetManager {
public:
  ~AssetManager();

  void SetLoader(const AssetLoader *cpAl);
  void Sync();
  assets::IAsset *AssetOf(const assets::AssetHandle &ah);

private:
  bool _initialized{false};
  uint64 _latestVersion{0};
  const AssetLoader *_cpAssetLoader;

private:
  struct typeRegistry {
    umap<assets::AssetHandle, IAssetPtr, assets::AssetHandleHash> assets;
    uset<assets::AssetHandle, assets::AssetHandleHash> badAssets;
  };

private:
  inline typeRegistry &registry() {
    static typeRegistry reg{};
    return reg;
  }

  std::expected<IAssetPtr, Error> loadImpl(const string &path);
};

} // namespace febundle::systems

#endif // INCLUDE_FEBUNDLE_CORE_SYSTEMS_ASSET_MANAGER_HPP_
