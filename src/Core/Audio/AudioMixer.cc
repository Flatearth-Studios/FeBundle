#define FE_DEBUG
#include "FeBundle/Core/Audio/AudioMixer.hpp"
#include "FeBundle/Core/Logger.hpp"
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <thread>

namespace febundle::core::audio {

Mixer::~Mixer() { Shutdown(); }

std::expected<void, Error> Mixer::Init() {
  if (_initialized) {
    FLOG_WARN("Mixer already initialized");
    return {};
  }

  if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
    FLOG_ERROR("failed to init SDL audio: {}", SDL_GetError());
    return std::unexpected{Error(ErrorName::InitializeAudioSubsystem)};
  }

  _deviceId = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
  if (_deviceId == 0) {
    FLOG_ERROR("failed to open audio device: {}", SDL_GetError());
    return std::unexpected{Error(ErrorName::CreateAudioDevice)};
  }

  // Query device format (dst)
  SDL_AudioSpec dev{};
  int sample_frames = 0;
  if (!SDL_GetAudioDeviceFormat(_deviceId, &dev, &sample_frames)) {
    FLOG_ERROR("SDL_GetAudioDeviceFormat failed: {}", SDL_GetError());
    SDL_CloseAudioDevice(_deviceId);
    _deviceId = 0;
    return std::unexpected{Error(ErrorName::CreateAudioDevice)};
  }
  _deviceSpec = dev;
  _deviceSampleFrames = sample_frames;

  const int bytes_per_sample = SDL_AUDIO_BYTESIZE(_deviceSpec.format);
  // keep roughly half a second queued as target
  _targetQueueBytes =
      (_deviceSpec.freq * bytes_per_sample * _deviceSpec.channels) / 2;
  if (_targetQueueBytes <= 0) {
    _targetQueueBytes =
        48000 * int(sizeof(float32)) * 2 / 2; // fallback ~0.5s stereo 48k
  }

  SDL_ResumeAudioDevice(_deviceId);
  _initialized = true;
  FLOG_INFO("Audio mixer initialized (device id: {})", _deviceId);

  // start feeder thread
  _running.store(true, std::memory_order_relaxed);
  _feeder = std::thread([this]() {
    while (_running.load(std::memory_order_relaxed)) {
      {
        std::scoped_lock lock(_mtx);
        for (auto it = _mapOfStreams.begin(); it != _mapOfStreams.end();) {
          auto &s = it->second;
          pushData(s);

          // if it's not looping and has finished draining, remove it
          if (!s.loop && !s.playing &&
              SDL_GetAudioStreamQueued(s.stream) == 0) {
            SDL_UnbindAudioStream(s.stream);
            SDL_DestroyAudioStream(s.stream);
            it = _mapOfStreams.erase(it);
          } else {
            ++it;
          }
        }
      }

      const uint32 delayMs = 10;
      SDL_Delay(delayMs);
    }
  });

  return {};
}

void Mixer::Shutdown() {
  if (!_initialized) {
    return;
  }

  _running.store(false, std::memory_order_relaxed);
  if (_feeder.joinable()) {
    _feeder.join();
  }

  {
    std::scoped_lock lock(_mtx);
    for (auto &kv : _mapOfStreams) {
      auto &s = kv.second;
      if (s.stream) {
        SDL_UnbindAudioStream(s.stream);
        SDL_DestroyAudioStream(s.stream);
        s.stream = nullptr;
      }
    }
    _mapOfStreams.clear();
  }

  if (_deviceId) {
    SDL_CloseAudioDevice(_deviceId);
    _deviceId = 0;
  }
  _initialized = false;
}

uint32 Mixer::Play(const Buffer &buffer, bool loop, float32 gain) {
  if (!_initialized) {
    FLOG_ERROR("Mixer not initialized");
    return 0;
  }

  std::scoped_lock lk(_mtx);
  const uint32 id = _nextId++;

  streamInfo s;
  s.buffer = buffer;
  s.loop = loop;
  s.gain = gain;
  s.playing = true;
  s.draining = false;
  s.readPosition = 0;

  SDL_AudioSpec src{};
  src.format = SDL_AUDIO_F32;
  src.channels = buffer.channels;
  src.freq = buffer.freq;

  s.stream = SDL_CreateAudioStream(&src, &_deviceSpec);
  if (!s.stream) {
    FLOG_ERROR("failed to create audio stream: {}", SDL_GetError());
    return 0;
  }

  SDL_BindAudioStream(_deviceId, s.stream);
  _mapOfStreams.emplace(id, std::move(s)); // insert first
  auto &stream = _mapOfStreams.at(id);     // reference the real one
  pushData(stream);                        // prime it
  FLOG_INFO("Playing audio stream {}", id);
  return id;
}

void Mixer::Stop(uint32 streamId) {
  std::scoped_lock lock(_mtx);
  auto it = _mapOfStreams.find(streamId);
  if (it == _mapOfStreams.end()) {
    return;
  }

  auto &s = it->second;
  s.playing = false; // feeder will clean up on next tick
  s.draining = false;
  if (s.stream) {
    SDL_UnbindAudioStream(s.stream);
    SDL_DestroyAudioStream(s.stream);
    s.stream = nullptr;
  }
  _mapOfStreams.erase(it);
  FLOG_DEBUG("Stopped stream {}", streamId);
}

void Mixer::pushData(streamInfo &s) {
  if (!s.stream) {
    return;
  }

  const auto totalSamples = s.buffer.samples.size();
  if (totalSamples == 0) {
    return;
  }

  constexpr std::size_t chunkSamples = 8192;
  const float32 *samples = s.buffer.samples.data();

  // Main refill loop: keep queue filled
  while (SDL_GetAudioStreamQueued(s.stream) < _targetQueueBytes) {

    // if we've reached end of buffer
    if (s.readPosition >= totalSamples) {
      if (s.loop) {
        // Seamless loop: restart immediately, no return
        s.readPosition = 0;
        FLOG_TRACE("Loop restart");
      } else {
        // not looping → stop once queue empties
        if (SDL_GetAudioStreamQueued(s.stream) <= 0) {
          s.playing = false;
          FLOG_TRACE("Audio stream reached real end");
        }
        return;
      }
    }

    std::size_t remaining = totalSamples - s.readPosition;
    std::size_t count = std::min(remaining, chunkSamples);

    _scratch.assign(samples + s.readPosition, samples + s.readPosition + count);

    for (auto &sample : _scratch) {
      sample *= s.gain;
    }

    if (!SDL_PutAudioStreamData(
            s.stream, _scratch.data(),
            static_cast<int>(_scratch.size() * sizeof(float32)))) {
      FLOG_ERROR("SDL_PutAudioStreamData failed: {}", SDL_GetError());
      s.playing = false;
      return;
    }

    s.readPosition += count;
  }
}

} // namespace febundle::core::audio
