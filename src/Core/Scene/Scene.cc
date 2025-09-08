#include "FeBundle/Core/Scene/Scene.hpp"

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

} // namespace febundle::scene
