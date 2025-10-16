#include "stduuid.h"
#include <random>
#define FE_DEBUG
#include "FeBundle/Core/Input/Inputs.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include "FeBundle/Core/Scene/Scene.hpp"
#include "FeBundle/Core/Systems/InputManager.hpp"

namespace febundle::scene {

Scene::Scene(enum SceneType type) : _type(type) {
  registerStore<Transform>();
  registerStore<Sprite>();
  registerStore<Input>();
  registerStore<Audio>();

  std::random_device rd;
  auto seedData = std::array<int32, std::mt19937::state_size>();
  std::generate(std::begin(seedData), std::end(seedData), std::ref(rd));
  std::seed_seq seq(std::begin(seedData), std::end(seedData));
  std::mt19937 generator(seq);
  uuids::uuid_random_generator gen(generator);
  _id = gen();
}

Entity Scene::Create() { return _next++; }

void Scene::Destroy(Entity e) {
  RemoveComponent<Transform>(e);
  RemoveComponent<Sprite>(e);
  RemoveComponent<Input>(e);
  RemoveComponent<Audio>(e);
}

enum SceneType Scene::Type() const {
  return _type;
}

string Scene::SceneId() const {
  return uuids::to_string(_id);
}

const umap<Entity, uset<Component>> &Scene::AccessAll() const {
  return _entityComponents;
}

void Scene::ProcessInputEvent(const systems::InputEvent *ie) {
  if (ie == nullptr) {
    return;
  }

  for (auto &[e, component] : _entityComponents) {
    if (!component.contains(Component::Input)) {
      continue;
    }

    Input *input = GetComponent<Input>(e);
    if (!input->keyMap.contains(ie->key)) {
      continue;
    }

    const bool isPressed = ie->keyState == core::input::KeyState::Pressed ||
                                   ie->keyState == core::input::KeyState::Held
                               ? true
                               : false;
    input->keyMap[ie->key] = isPressed;
  }
}

} // namespace febundle::scene
