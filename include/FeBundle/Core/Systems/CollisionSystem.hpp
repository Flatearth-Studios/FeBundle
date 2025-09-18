#ifndef INCLUDE_FEBUNDLE_CORE_SYSTEMS_COLLISION_SYSTEM_HPP_
#define INCLUDE_FEBUNDLE_CORE_SYSTEMS_COLLISION_SYSTEM_HPP_

#include "FeBundle/Core/Events/EventQueue.hpp"
#include "FeBundle/Core/Scene/Entity.hpp"
#include "FeBundle/Core/Scene/Scene.hpp"
#include <functional>
namespace febundle::systems {

struct GridCell {
  std::vector<scene::Entity> entities;
};

enum class CollisionPhase {
  Enter,
  Stay,
  Exit,
};

struct CollisionEvent {
  scene::Entity e1, e2;
  CollisionPhase phase;
};

class CollisionSystem {
public:
  CollisionSystem(float32 cellSize = 64.0f, int64 maxCallbacksAllowed = 500);
  void Update(scene::Scene &scene);

  void OnCollision(std::function<void(const CollisionEvent &)> callback,
                   CollisionPhase phase = CollisionPhase::Enter);

  void OnCollisionEnter(std::function<void(const CollisionEvent &)> callback);
  void OnCollisionStay(std::function<void(const CollisionEvent &)> callback);
  void OnCollisionExit(std::function<void(const CollisionEvent &)> callback);

private:
  static bool checkCollision(const scene::Transform &transform1,
                             const scene::Collider &collider1,
                             const scene::Transform &transform2,
                             const scene::Collider &collider2);

  int64 hashCell(int32 cx, int32 cy) const;
  std::pair<int32, int32> worldToCell(float32 x, float32 y) const;

  void dispatchEvents();

private:
  const int64 _cMaxCallbacks;
  int64 _enterCallbackCount, _stayCallbackCount, _exitCallbackCount;
  float32 _cellSize;
  events::EventQueue<CollisionEvent> _eventQ;
  std::vector<std::function<void(const CollisionEvent &)>> _enterCallbacks;
  std::vector<std::function<void(const CollisionEvent &)>> _stayCallbacks;
  std::vector<std::function<void(const CollisionEvent &)>> _exitCallbacks;
  umap<int64, GridCell> _grid;
};

} // namespace febundle::systems

#endif // INCLUDE_FEBUNDLE_CORE_SYSTEMS_COLLISION_SYSTEM_HPP_
