#ifndef INCLUDE_FEBUNDLE_CORE_GAME_TYPES_HPP_
#define INCLUDE_FEBUNDLE_CORE_GAME_TYPES_HPP_

#include "FeBundle/Core/Scene/Scene.hpp"
#include "FeBundle/Core/Systems/CollisionSystem.hpp"
#include "FeBundle/Core/Window/Window.hpp"
#include "FeBundle/Core/GameBridge.hpp"
#include <functional>

namespace febundle {

const int64 MaxScenes = 300;

struct Game {
  window::WindowSpecs windowSpecs;

  void *pState = nullptr;
  std::vector<scene::Scene> scenes;
  systems::CollisionSystem collisionSystem;
  systems::AssetLoader assetLoader;
  GameBridge *pBridge;
  std::size_t activeSceneIndex;

  std::function<bool(struct Game &)> Initialize;
  std::function<bool(struct Game &, float32)> Update;
  std::function<bool(struct Game &, uint32 width, uint32 height)> OnResize;

  Game()
      : Initialize(nullptr), Update(nullptr), OnResize(nullptr),
        activeSceneIndex(-1) {
    scenes.reserve(MaxScenes);
  }
};

} // namespace febundle

#endif // INCLUDE_FEBUNDLE_CORE_GAME_TYPES_HPP_
