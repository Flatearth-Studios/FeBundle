#ifndef INCLUDE_FEBUNDLE_CORE_AUDIO_AUDIO_MIXER_HPP_
#define INCLUDE_FEBUNDLE_CORE_AUDIO_AUDIO_MIXER_HPP_

#include "FeBundle/Core/Defines.hpp"
#include <SDL3/SDL_audio.h>
#include <mutex>
#include <thread>

namespace febundle::core::audio {

struct Buffer {
  int32 freq{48000};
  int32 channels{2};
  std::vector<float32> samples;
};

class Mixer {
public:
  ~Mixer();
  std::expected<void, Error> Init();
  void Shutdown();

  uint32 Play(const Buffer &buffer, bool loop = false, float32 gain = 1.0f);
  void Stop(uint32 streamId);

private:
  struct streamInfo {
    SDL_AudioStream *stream{nullptr};
    Buffer buffer{};
    bool loop{false};
    bool playing{false};
    bool draining{false};
    float32 gain{1.0f};
    std::size_t readPosition{0};
  };

private:
  void pushData(streamInfo &s);

private:
  SDL_AudioDeviceID _deviceId{0};
  SDL_AudioSpec _deviceSpec{};
  int32 _deviceSampleFrames{0};
  int32 _targetQueueBytes{0};

  atomic_bool _running{false};
  std::thread _feeder;
  std::mutex _mtx;

  uint32 _nextId{1};
  umap<uint32, streamInfo> _mapOfStreams;

  std::vector<float32> _scratch;
  bool _initialized{false};
};

}; // namespace febundle::core::audio

#endif // INCLUDE_FEBUNDLE_CORE_AUDIO_AUDIO_MIXER_HPP_
