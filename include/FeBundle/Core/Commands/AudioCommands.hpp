#ifndef INCLUDE_FEBUNDLE_CORE_COMMANDS_AUDIO_COMMANDS_HPP_
#define INCLUDE_FEBUNDLE_CORE_COMMANDS_AUDIO_COMMANDS_HPP_

#include "FeBundle/Core/Assets/Common.hpp"
#include "FeBundle/Core/Defines.hpp"

namespace febundle::commands {

struct PlaySoundCommand {
  assets::AssetHandle assetHandle;
  float32 volume;
};

struct StopSoundCommand {
  assets::AssetHandle assetHandle;
};

struct PlayBGMCommand {
  assets::AssetHandle assetHandle;
  bool loop;
};

struct StopBGMCommand {
  assets::AssetHandle assetHandle; 
};

}

#endif // INCLUDE_FEBUNDLE_CORE_COMMANDS_AUDIO_COMMANDS_HPP_
