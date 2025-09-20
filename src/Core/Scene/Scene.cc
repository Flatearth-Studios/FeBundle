#include "FeBundle/Core/Input/Inputs.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#define FE_DEBUG
#include "FeBundle/Core/Scene/Scene.hpp"
#include "FeBundle/Core/Systems/InputManager.hpp"

namespace febundle::scene {

Scene::Scene() {
  registerStore<Transform>();
  registerStore<Sprite>();
  registerStore<Input>();
}

Entity Scene::Create() { return _next++; }

void Scene::Destroy(Entity e) {
  RemoveComponent<Transform>(e);
  RemoveComponent<Sprite>(e);
  RemoveComponent<Input>(e);
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
