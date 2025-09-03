#include "FeBundle/Core/Scene/Scene.hpp"
#include "FeBundle/Core/Systems/AssetManager.hpp"

namespace febundle::scene {

void Scene::SetAssetManager(systems::AssetManager *am) {
  _pAssetMgr = am;
}

void Scene::Destroy(Entity e) {
  _transformStore.Remove(e);
  _spriteStore.Remove(e);
}

Entity Scene::CreateSprite(TextureHandle tex, const string &path, float32 x, float32 y, float32 w,
                           float32 h) {
  Entity e = create();
  _spriteStore.Emplace(e, Sprite(e, tex, w, h));
  _transformStore.Emplace(e, Transform(x, y));
  _pAssetMgr->RegisterTexture(tex, path);
  return e;
}

const Store<Transform> &Scene::Transforms() const { return _transformStore; }

const Store<Sprite> &Scene::Sprites() const { return _spriteStore; }

std::expected<const Transform *, Error> Scene::TransformOf(Entity e) const {
  const Transform *ptr = _transformStore.Get(e);
  if (ptr == nullptr) {
    return std::unexpected{Error(ErrorName::NullReference)};
  }
  return ptr;
}

std::expected<const Sprite *, Error> Scene::SpriteOf(Entity e) const {
  const Sprite *ptr = _spriteStore.Get(e);
  if (ptr == nullptr) {
    return std::unexpected{Error(ErrorName::NullReference)};
  }
  return ptr;
}

std::expected<Transform *, Error> Scene::TransformOf(Entity e) {
  Transform *ptr = _transformStore.Get(e);
  if (ptr == nullptr) {
    return std::unexpected{Error(ErrorName::NullReference)};
  }
  return ptr;
}

std::expected<Sprite *, Error> Scene::SpriteOf(Entity e) {
  Sprite *ptr = _spriteStore.Get(e);
  if (ptr == nullptr) {
    return std::unexpected{Error(ErrorName::NullReference)};
  }
  return ptr;
}

Entity Scene::create() { return _next++; }

} // namespace febundle::scene
