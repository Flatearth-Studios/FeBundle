#ifndef INCLUDE_FEBUNDLE_CORE_SYSTEMS_COLLISION_SYSTEM_HPP_
#define INCLUDE_FEBUNDLE_CORE_SYSTEMS_COLLISION_SYSTEM_HPP_

#include "FeBundle/Core/Events/EventQueue.hpp"
#include "FeBundle/Core/Scene/Entity.hpp"
#include "FeBundle/Core/Scene/Scene.hpp"
namespace febundle::systems {

struct GridCell {
  std::vector<scene::Entity> entities;
};

struct CollisionEvent {
  scene::Entity e1, e2;
};

class CollisionSystem {
public:
  CollisionSystem(float32 cellSize = 64.0f);
  void Update(scene::Scene &scene, events::EventQueue<CollisionEvent> &queue);

private:
  static bool checkCollision(const scene::Transform &transform1,
                             const scene::Collider &collider1,
                             const scene::Transform &transform2,
                             const scene::Collider &collider2);

  int64 hashCell(int32 cx, int32 cy) const;
  std::pair<int32, int32> worldToCell(float32 x, float32 y) const;

private:
  float32 _cellSize;
  umap<int64, GridCell> _grid;
};

} // namespace febundle::systems

#endif // INCLUDE_FEBUNDLE_CORE_SYSTEMS_COLLISION_SYSTEM_HPP_
