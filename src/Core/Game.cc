#include "FeBundle/Core/GameTypes.hpp"
#include "FeBundle/Core/Logger.hpp"
#include "FeBundle/Core/Scene/Scene.hpp"

namespace febundle {

Game::Game() : Initialize(nullptr), Update(nullptr), OnResize(nullptr) {
  scenes.reserve(MaxScenes);
}

void Game::LoadScene(const string &alias, scene::Scene scene, float32 width, float32 height) {
  const string id = scene.SceneId();

  if (pBridge != nullptr) {
    pBridge->LoadScene(scene, width, height);
  }

  if (scenes.contains(id)) {
    scenes[id] = std::move(scene);
  } else {
    scenes.emplace(id, std::move(scene));
  }

  mapOfSceneIds[alias] = id;
}


const scene::Scene *Game::SceneReference(const string &alias) const {
  auto it = mapOfSceneIds.find(alias);
  if (it == mapOfSceneIds.end()) {
    return nullptr;
  }

  const string id = it->second;
  if (!scenes.contains(id)) {
    FLOG_WARN("somehow alias leads to id, but no scene under that id was found");
    return nullptr;
  } 

  return &scenes.at(id); 
}

scene::Scene *Game::SceneReference(const string &alias) {
  auto it = mapOfSceneIds.find(alias);
  if (it == mapOfSceneIds.end()) {
    return nullptr;
  }

  const string id = it->second;
  if (!scenes.contains(id)) {
    FLOG_WARN("somehow alias leads to id, but no scene under that id was found");
    return nullptr;
  }

  return &scenes.at(id);
}

} // namespace febundle
