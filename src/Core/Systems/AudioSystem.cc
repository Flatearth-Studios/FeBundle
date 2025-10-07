#define FE_DEBUG
#include "FeBundle/Core/Systems/AudioSystem.hpp"
#include "FeBundle/Core/Assets/Common.hpp"
#include "FeBundle/Core/Audio/AudioMixer.hpp"
#include "FeBundle/Core/Commands/AudioCommands.hpp"
#include "FeBundle/Core/Events/AssetLoadEvent.hpp"
#include "FeBundle/Core/Events/EventQueue.hpp"
#include "FeBundle/Core/Events/GameCommandEvent.hpp"
#include "FeBundle/Core/Logger.hpp"
#include <SDL3/SDL_audio.h>

namespace febundle::systems {

AudioSystem::AudioSystem(core::events::EventBus &evtBus) : _eventBus(evtBus) {
  subscribeToEvents();
}

std::expected<void, Error> AudioSystem::Init() { return _mixer.Init(); }

void AudioSystem::Play(const assets::AssetHandle &handle, float32 volume) {
  if (_handleToId.contains(handle)) {
    return;
  }

  auto bufferRes = loadWav(handle.path.string());
  if (!bufferRes.has_value()) {
    FLOG_ERROR("could not load wav for path: {}", handle.path.string());
    return;
  }
  const auto buffer = bufferRes.value();
  const auto id = _mixer.Play(buffer, true);
  _handleToId.emplace(handle, id);
}

void AudioSystem::Stop(const assets::AssetHandle &handle) {
  auto it = _handleToId.find(handle);
  if (it == _handleToId.end()) {
    return;
  }

  const auto id = it->second;
  _mixer.Stop(id);
  _handleToId.erase(handle);
}

std::expected<core::audio::Buffer, Error>
AudioSystem::loadWav(const string &path) {
  using core::audio::Buffer;

  SDL_AudioSpec spec;
  uint8 *data = nullptr;
  uint32 length = 0;

  if (!SDL_LoadWAV(path.c_str(), &spec, &data, &length)) {
    FLOG_ERROR("failed to load wav: {}", SDL_GetError());
    return std::unexpected{Error(ErrorName::LoadAudio)};
  }

  Buffer buffer{
      .freq = spec.freq,
      .channels = spec.channels,
  };

  const std::size_t sampleCount = length / sizeof(int16);
  buffer.samples.resize(sampleCount);

  const int16 *src = reinterpret_cast<int16 *>(data);
  const float32 pcm = 32768.0f;
  for (std::size_t i = 0; i < sampleCount; i++) {
    buffer.samples[i] = static_cast<float32>(src[i]) / pcm;
  }

  SDL_free(data);
  return buffer;
}

void AudioSystem::subscribeToEvents() {
  core::events::EventSubscription<core::events::GameCommandEvent> gameCmdSub{
      .subscriber = "AudioSystem",
      .callback = [&](const core::events::GameCommandEvent &evt)
          -> std::expected<void, Error> {
        if (evt.commandKind != core::events::GameCommandKind::Audio) {
          return {};
        }

        if (evt.name == "PlaySound") {
          auto *cmd = static_cast<commands::PlaySoundCommand *>(evt.payload);
          this->Play(cmd->assetHandle, cmd->volume);
        } else if (evt.name == "StopSound") {
          auto *cmd = static_cast<commands::StopSoundCommand *>(evt.payload);
          this->Stop(cmd->assetHandle);
        }

        return {};
      },
  };

  core::events::EventSubscription<core::events::AssetLoadEvent> loadEvtSub{
      .subscriber = "AudioSystem",
      .callback = [&](const core::events::AssetLoadEvent &evt)
          -> std::expected<void, Error> {
        if (evt.assetType != assets::AssetType::Audio) {
          return {};
        }

        const auto handle = evt.assetHandle;
        const auto path = handle.path.string();

        // Stop old sound if it’s playing
        if (_handleToId.contains(handle)) {
          FLOG_INFO("Reloading audio asset {}", path);
          Stop(handle);
        }

        // Reload buffer and play again
        auto bufferRes = loadWav(path);
        if (!bufferRes.has_value()) {
          FLOG_ERROR("Failed to reload audio asset {}", path);
          return {};
        }

        const auto newBuffer = bufferRes.value();
        const auto id = _mixer.Play(newBuffer, true);
        _handleToId[handle] = id;

        FLOG_INFO("Audio asset {} reloaded and playing", path);
        return {};
      },
  };

  if (auto res = _eventBus.Subscribe(gameCmdSub); !res.has_value()) {
    FLOG_ERROR("failed to subscribe to GameCommandEvents");
  }
  if (auto res = _eventBus.Subscribe(loadEvtSub); !res.has_value()) {
    FLOG_ERROR("failed to subscribe to AssetLoadEvent");
  }
}

} // namespace febundle::systems
