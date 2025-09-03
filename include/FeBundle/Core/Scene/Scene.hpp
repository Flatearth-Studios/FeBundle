#ifndef INCLUDE_FEBUNDLE_CORE_SCENE_SCENE_HPP_
#define INCLUDE_FEBUNDLE_CORE_SCENE_SCENE_HPP_

#include "FeBundle/Core/Scene/Entity.hpp"
#include "FeBundle/Core/Scene/Store.hpp"
#include "FeBundle/Core/Scene/Components.hpp"
#include "FeBundle/Core/Systems/AssetManager.hpp"

namespace febundle::scene {

class Scene {
public:
  void SetAssetManager(systems::AssetManager *am);
  void Destroy(Entity e);
  Entity CreateSprite(TextureHandle tex, const string& path, float32 x, float32 y, float32 w,
                      float32 h);

  const Store<Transform> &Transforms() const;
  const Store<Sprite> &Sprites() const;

  std::expected<const Transform *, Error> TransformOf(Entity e) const;
  std::expected<const Sprite *, Error> SpriteOf(Entity e) const;

  std::expected<Transform *, Error> TransformOf(Entity e);
  std::expected<Sprite *, Error> SpriteOf(Entity e);

private:
  Entity create();

private:
  Store<Transform> _transformStore;
  Store<Sprite> _spriteStore;
  Entity _next{0};
  systems::AssetManager *_pAssetMgr;
};

} // namespace febundle::scene

#endif // INCLUDE_FEBUNDLE_CORE_SCENE_SCENE_HPP_
