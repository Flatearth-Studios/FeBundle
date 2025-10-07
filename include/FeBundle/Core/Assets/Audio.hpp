#ifndef INCLUDE_FEBUNDLE_CORE_ASSETS_AUDIO_HPP_
#define INCLUDE_FEBUNDLE_CORE_ASSETS_AUDIO_HPP_

#include "FeBundle/Core/Assets/Common.hpp"

namespace febundle::assets {

class Audio : public IAsset {
public:
  const assets::AssetType &Type() const override { return _handle.type; }
  uint64 Id() const override { return _handle.id; }

  const std::filesystem::path &Path() const { return _handle.path; }
};

} // namespace febundle::assets

#endif // INCLUDE_FEBUNDLE_CORE_ASSETS_AUDIO_HPP_
