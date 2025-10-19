#include "FeBundle/Core/Scene/Entity.hpp"
#include "stduuid.h"
#include <random>
#define FE_DEBUG
#include "FeBundle/Core/Input/Inputs.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include "FeBundle/Core/Scene/Scene.hpp"
#include "FeBundle/Core/Systems/InputManager.hpp"

static constexpr int32 scMaxEntitiesAllowed = 50000;

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

Entity Scene::Create(entity::Tag tag, const string &name) {
  Entity e = _next++;
  _entityMetadata.emplace(e, EntityMetadata{
      .name = name,
      .tag = tag,
    });

  return e; 
}

void Scene::Destroy(Entity e) {
  RemoveComponent<Transform>(e);
  RemoveComponent<Sprite>(e);
  RemoveComponent<Input>(e);
  RemoveComponent<Audio>(e);
  if (_entityMetadata.contains(e)) {
    std::size_t _ = _entityMetadata.erase(e);
  }
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

std::vector<Entity> Scene::FindByTag(entity::Tag tag) const {
  std::vector<Entity> out;
  out.reserve(scMaxEntitiesAllowed);

  for (const auto &[entity, entityMetadata] : _entityMetadata) {
    if (entityMetadata.tag == tag) {
      out.push_back(entity);
    }
  }

  return out;
}

std::vector<Entity> Scene::FindByName(const string &name) const {
  std::vector<Entity> out;
  out.reserve(scMaxEntitiesAllowed);

  for (const auto &[entity, entityMetadata] : _entityMetadata) {
    if (entityMetadata.name == name) {
      out.push_back(entity);
    }
  }

  return out;
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
