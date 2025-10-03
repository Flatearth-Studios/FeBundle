#ifndef INCLUDE_FEBUNDLE_CORE_ASSETS_COMMON_HPP_
#define INCLUDE_FEBUNDLE_CORE_ASSETS_COMMON_HPP_

#include "FeBundle/Core/Defines.hpp"
#include <SDL3/SDL_render.h>

namespace febundle::assets {

enum class AssetType {
  Texture,
  Audio,
};

struct AssetHandle {
  uint64 id{0};
  AssetType type;
  fs::path path;

  AssetHandle() : type(AssetType::Texture) {}
  AssetHandle(uint64 id) : id(id), type(AssetType::Texture) {}
  AssetHandle(uint64 id, AssetType type) : id(id), type(type) {}
  AssetHandle(uint64 id, AssetType type, const fs::path &path)
      : id(id), type(type), path(path) {}

  bool operator==(const AssetHandle &other) const {
    return id == other.id && type == other.type && path == other.path;
  }
};

struct AssetHandleHash {
  std::size_t operator()(const AssetHandle &ah) const {
    using UT = std::underlying_type_t<AssetType>;
    return std::hash<uint64>{}(ah.id) ^
           (std::hash<UT>{}(static_cast<UT>(ah.type)) << 1) ^
           (std::hash<std::string>{}(ah.path.string()) << 2);
  }
};

class IAsset {
public:
  virtual ~IAsset() = default;
  virtual const assets::AssetType &Type() const = 0;
  virtual uint64 Id() const = 0;

  virtual const assets::AssetHandle &AssetHandle() const { return _handle; }

protected:
  struct assets::AssetHandle _handle;
};

} // namespace febundle::assets

#endif // INCLUDE_FEBUNDLE_CORE_ASSETS_COMMON_HPP_
