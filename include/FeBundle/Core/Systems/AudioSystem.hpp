#ifndef INCLUDE_FEBUNDLE_CORE_SYSTEMS_AUDIO_SYSTEM_HPP_
#define INCLUDE_FEBUNDLE_CORE_SYSTEMS_AUDIO_SYSTEM_HPP_

#include "FeBundle/Core/Assets/Common.hpp"
#include "FeBundle/Core/Audio/AudioMixer.hpp"
#include "FeBundle/Core/Events/EventBus.hpp"

namespace febundle::systems {

class AudioSystem {
public:
  AudioSystem(core::events::EventBus &evtBus);
  std::expected<void, Error> Init();

  void Play(const assets::AssetHandle &handle, float32 volume);
  void Stop(const assets::AssetHandle &handle);

private:
  std::expected<core::audio::Buffer, Error> loadWav(const string &path);
  void subscribeToEvents();

private:
  core::audio::Mixer _mixer;
  core::events::EventBus &_eventBus;
  uset<assets::AssetHandle, assets::AssetHandleHash> _loadedSfx;
  umap<assets::AssetHandle, uint32, assets::AssetHandleHash> _handleToId;
};

} // namespace febundle::systems

#endif // INCLUDE_FEBUNDLE_CORE_SYSTEMS_AUDIO_SYSTEM_HPP_
