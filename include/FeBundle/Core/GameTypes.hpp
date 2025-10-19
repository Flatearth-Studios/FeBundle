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

  bool isRunning{false}, isSuspended{true};

  void *pState = nullptr;
  umap<string, string> mapOfSceneIds;
  umap<string, scene::Scene> scenes;
  systems::CollisionSystem collisionSystem;
  systems::AssetLoader assetLoader;
  GameBridge *pBridge;

  std::function<bool(struct Game &)> Initialize;
  std::function<bool(struct Game &, float32)> Update;
  std::function<bool(struct Game &, uint32 width, uint32 height)> OnResize;

  Game();

  void LoadScene(const string &alias, scene::Scene scene);
  const scene::Scene *SceneReference(const string &alias) const;
  scene::Scene *SceneReference(const string &alias);
};

} // namespace febundle

#endif // INCLUDE_FEBUNDLE_CORE_GAME_TYPES_HPP_
