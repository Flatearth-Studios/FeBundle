#ifndef INCLUDE_FEBUNDLE_CORE_SYSTEMS_ASSET_MANAGER_HPP_
#define INCLUDE_FEBUNDLE_CORE_SYSTEMS_ASSET_MANAGER_HPP_

#include "FeBundle/Core/Assets/Common.hpp"
#include "FeBundle/Core/Defines.hpp"
#include "FeBundle/Core/Memory/Memory.hpp"
#include "FeBundle/Core/Systems/AssetLoader.hpp"
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
#include <functional>

namespace febundle::systems {

using IAssetPtr =
    std::unique_ptr<assets::IAsset, memory::PolyDeleter<assets::IAsset>>;

using AssetLoaderFn =
    std::function<std::expected<IAssetPtr, Error>(const string &)>;

class AssetManager {
public:
  AssetManager();
  ~AssetManager();

  void SetLoader(const AssetLoader *cpAl);
  void Sync();
  assets::IAsset *AssetOf(const assets::AssetHandle &ah);

private:
  bool _initialized{false};
  uint64 _latestVersion{0};
  const AssetLoader *_cpAssetLoader;
  umap<assets::AssetType, AssetLoaderFn> _loaderImplementations;

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

  std::expected<IAssetPtr, Error> loadImpl(const string &path,
                                           assets::AssetType type);

  static std::expected<IAssetPtr, Error> textureLoader(const string &path);
};

} // namespace febundle::systems

#endif // INCLUDE_FEBUNDLE_CORE_SYSTEMS_ASSET_MANAGER_HPP_
