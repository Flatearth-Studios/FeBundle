#ifndef INCLUDE_FEBUNDLE_CORE_EVENTS_ASSET_LOAD_EVENT_HPP_
#define INCLUDE_FEBUNDLE_CORE_EVENTS_ASSET_LOAD_EVENT_HPP_

#include "FeBundle/Core/Assets/Common.hpp"
namespace febundle::core::events {

struct AssetLoadEvent {
  assets::AssetHandle assetHandle;
  assets::AssetType assetType;
  assets::IAsset *asset;
};

};

#endif // INCLUDE_FEBUNDLE_CORE_EVENTS_ASSET_LOAD_EVENT_HPP_
